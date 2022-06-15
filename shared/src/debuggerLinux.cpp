#ifdef PAVO_UNIX

#include "debugger.h"
#include "misc.h"
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <genericType.h>
#include <vector>
#include "stringutils.h"
#include <sys/personality.h>
#include <sys/user.h>
#include "fmt/core.h"
#include "fmt/format.h"
#include <fstream>

#include "elf++.hh"
#include "dwarf++.hh"
#include <unistd.h>

#pragma region break point

static constexpr std::uint64_t BOTTOM_BYTE_MASK = 0xff;

void breakpoint_t::enable()
{
        auto data = ptrace(PTRACE_PEEKDATA, process, addr, nullptr);

        saved_data = static_cast<std::uint8_t>(data & BOTTOM_BYTE_MASK);

        std::uint64_t int3_op  = 0xcc;
        std::uint64_t mask     = ~BOTTOM_BYTE_MASK;
        std::uint64_t new_data = ((data & mask) | int3_op);
        ptrace(PTRACE_POKEDATA, process, addr, new_data);

        enabled = true;
}

void breakpoint_t::disable()
{
        auto data = ptrace(PTRACE_PEEKDATA, process, addr, nullptr);

        std::uint64_t restored_data = ((data & (~BOTTOM_BYTE_MASK)) | saved_data);
        ptrace(PTRACE_POKEDATA, process, addr, restored_data);

        enabled = false;
}

bool breakpoint_t::is_enabled() const
{
        return enabled;
}

std::uint64_t breakpoint_t::get_addr() const
{
        return addr;
}

#pragma endregion

#pragma region debugger

bool debugger_t::start(const std::string progName)
{
        const PROCESS child = debugger_t::run_program(progName);
        if(child == 0)
        {
                return 0;
        }

        this->progName = progName;
        this->process  = child;
        const auto fd  = open(progName.c_str(), O_RDONLY);
        elf            = elf_wrapper::elf{elf_wrapper::create_mmap_loader(fd)};
        dwarf          = dwarf_wrapper::dwarf{dwarf_wrapper::elf::create_loader(elf)};

        return true;
}

void debugger_t::continue_execution()
{
        step_over_breakpoint();
        ptrace(PTRACE_CONT, process, nullptr, nullptr);
        wait_for_signal();
}

void debugger_t::set_breakpoint(const std::uint64_t addr)
{
        fmt::print("Breakpoint set at address {:#018x}\n", addr);

        breakpoint_t breakpoint{process, addr};
        breakpoint.enable();
        breakpoints.insert(std::make_pair(addr, breakpoint));
}

void debugger_t::dump_registers()
{
        for(const auto& reg_desc : register_descriptors)
        {
                fmt::print("{} {:#018x}\n", reg_desc.name,
                           *get_register_value(process, reg_desc.r));
        }
}

std::uint64_t debugger_t::read_memory(const std::uint64_t addr)
{
        return ptrace(PTRACE_PEEKDATA, process, addr, nullptr);
}

void debugger_t::write_memory(const std::uint64_t addr, const std::uint64_t value)
{
        ptrace(PTRACE_POKEDATA, process, addr, value);
}

std::uint64_t debugger_t::get_pc()
{
        return *get_register_value(process, Reg::rip);
}

void debugger_t::set_pc(const std::uint64_t val)
{
        set_register_value(process, Reg::rip, val);
}

void debugger_t::step_over_breakpoint()
{
        if(contains(breakpoints, get_pc()))
        {
                auto& bp = breakpoints.at(get_pc());

                if(bp.is_enabled())
                {
                        bp.disable();
                        ptrace(PTRACE_SINGLESTEP, process, nullptr, nullptr);
                        wait_for_signal();
                        bp.enable();
                }
        }
}

void debugger_t::wait_for_signal()
{
        int wait_status;
        waitpid(process, &wait_status, 0);

        const auto siginfo = get_signal_info();

        switch(siginfo.si_signo)
        {
        case SIGTRAP:
        {
                handle_sigtrap(siginfo);
                break;
        }
        case SIGSEGV:
        {
                fmt::print("Segfault. Reason: {}\n", siginfo.si_code);
                break;
        }
        default:
        {
                fmt::print("Got signal: {}\n", strsignal(siginfo.si_signo));
        }
        }
}

void debugger_t::handle_sigtrap(siginfo_t siginfo)
{
        switch(siginfo.si_code)
        {
        case SI_KERNEL:
        case TRAP_BRKPT:
        {
                set_pc(get_pc() - 1);
                fmt::print("Hit breakpoint at address: {:#018x}\n", get_pc());

                auto offset_pc  = offset_load_address(get_pc());
                auto line_entry = *get_line_entry_from_pc(offset_pc);

                const std::string str   = line_entry.getFilePath();
                const unsigned line_num = line_entry.getEntryLine();

                print_source(str, line_num);
                return;
        }
        case TRAP_TRACE:
        {
                return;
        }
        default:
        {
                fmt::print("Unknown SIGTRAP code: {}\n", siginfo.si_code);
                return;
        }
        }
}

void debugger_t::init_load_addr()
{
        if(elf.get_hdr().type == elf::et::dyn)
        {
                std::ifstream map("/proc/" + std::to_string(process) + "/maps");

                std::string addr;
                std::getline(map, addr, '-');

                load_address = std::stoull(addr, 0, 16);
        }
}

std::uint64_t debugger_t::offset_load_address(const std::uint64_t addr)
{
        return addr - load_address;
}

std::optional<dwarf::die> debugger_t::get_function_from_pc(const std::uint64_t pc)
{
        for(auto& comp_unit : dwarf.compilation_units())
        {
                const auto pc_range = dwarf::die_pc_range(comp_unit.root());
                if(!pc_range.contains(pc))
                {
                        continue;
                }

                for(const auto& die : comp_unit.root())
                {
                        if(die.tag == dwarf::DW_TAG::subprogram)
                        {
                                if(dwarf::die_pc_range(die).contains(pc))
                                {
                                        return die;
                                }
                        }
                }
        }

        return std::nullopt;
}

std::optional<dwarf_wrapper::line_table>
debugger_t::get_line_entry_from_pc(const std::uint64_t pc)
{
        for(auto& comp_unit : dwarf.compilation_units())
        {
                const auto pc_range = dwarf::die_pc_range(comp_unit.root());
                if(!pc_range.contains(pc))
                {
                        continue;
                }

                auto& lt = comp_unit.get_line_table();
                auto it  = lt.find_address(pc);

                if(it != lt.end())
                {
                        dwarf_wrapper::line_table d(it);
                        return d;
                }

                throw std::out_of_range{"Line entry not found!"};
                return std::nullopt;
        }

        throw std::out_of_range{"Line entry not found!"};
        return std::nullopt;
}

PROCESS debugger_t::run_program(const std::string& path)
{
        const PID child_pid = fork();
        if(child_pid < 0)
        {
                perror("fork");
                return 0;
        }

        if(child_pid == 0)
        {
                personality(ADDR_NO_RANDOMIZE);
                const auto ret = ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
                if(ret < 0)
                {
                        perror("ptrace");
                        exit(EXIT_FAILURE);
                }

                const char* prog_str = path.c_str();
                execl(prog_str, prog_str, nullptr);

                perror("execl");
                exit(EXIT_FAILURE);
        }

        return child_pid;
}

void debugger_t::print_source(const std::string& filename, const unsigned line,
                              const unsigned context)
{
        std::ifstream file(filename);

        const int start = line <= context ? 1 : line - context;
        const int end   = line + context + (line < context ? context - line : 0) + 1;

        char c;
        std::uint64_t current_line = 1;

        while(current_line != start && file.get(c))
        {
                if(c == '\n')
                {
                        ++current_line;
                }
        }

        fmt::print(fmt::runtime(current_line == line ? "> " : "  "));

        while(current_line <= end && file.get(c))
        {
                fmt::print("{}", c);
                if(c == '\n')
                {
                        ++current_line;
                        fmt::print(fmt::runtime(current_line == line ? "> " : "  "));
                }
        }

        fmt::print("\n");
}

siginfo_t debugger_t::get_signal_info()
{
        siginfo_t info;
        ptrace(PTRACE_GETSIGINFO, process, nullptr, &info);
        return info;
}

void debugger_t::single_step_instruction()
{
        ptrace(PTRACE_SINGLESTEP, process, nullptr, nullptr);
        wait_for_signal();
}

void debugger_t::single_step_instruction_check_br()
{
        if(contains(breakpoints, get_pc()))
        {
                step_over_breakpoint();
        }
        else
        {
                single_step_instruction();
        }
}

void debugger_t::remove_breakpoint(const std::uint64_t addr)
{
        if(breakpoints.at(addr).is_enabled())
        {
                breakpoints.at(addr).disable();
        }

        breakpoints.erase(addr);
}

void debugger_t::step_out()
{
        auto frame_pointer  = *get_register_value(process, Reg::rbp);
        auto return_address = read_memory(frame_pointer + 8);

        bool should_remove_breakpoint = false;
        if(!contains(breakpoints, return_address))
        {
                set_breakpoint(return_address);
                should_remove_breakpoint = true;
        }

        continue_execution();

        if(should_remove_breakpoint)
        {
                remove_breakpoint(return_address);
        }
}

std::uint64_t debugger_t::get_offset_pc()
{
        return offset_load_address(get_pc());
}

void debugger_t::step_in()
{
        auto line = get_line_entry_from_pc(get_offset_pc())->getEntryLine();

        while(get_line_entry_from_pc(get_offset_pc())->getEntryLine() == line)
        {
                single_step_instruction_check_br();
        }

        auto line_entry = *get_line_entry_from_pc(get_offset_pc());
        print_source(line_entry.getFilePath(), line_entry.getEntryLine());
}

uint64_t debugger_t::offset_dwarf_address(const uint64_t addr)
{
        return addr + load_address;
}

void debugger_t::step_over()
{
        auto func       = get_function_from_pc(get_offset_pc());
        auto func_entry = dwarf::at_low_pc(*func);
        auto func_end   = dwarf::at_high_pc(*func);

        auto line       = get_line_entry_from_pc(func_entry);
        auto start_line = get_line_entry_from_pc(get_offset_pc());

        std::vector<std::intptr_t> to_delete{};

        while(line->getAddress() < func_end)
        {
                auto load_address = offset_dwarf_address(line->getAddress());
                if(line->getAddress() != start_line->getAddress() &&
                   !breakpoints.count(load_address))
                {
                        set_breakpoint(load_address);
                        to_delete.push_back(load_address);
                }
                ++(*line);
        }

        auto frame_pointer  = *get_register_value(process, Reg::rbp);
        auto return_address = read_memory(frame_pointer + 8);
        if(!breakpoints.count(return_address))
        {
                set_breakpoint(return_address);
                to_delete.push_back(return_address);
        }

        continue_execution();

        for(auto addr : to_delete)
        {
                remove_breakpoint(addr);
        }
}

#pragma endregion

#endif
