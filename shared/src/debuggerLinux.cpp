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

                const auto offset_pc  = offset_load_address(get_pc());
                const auto line_entry = *get_line_entry_from_pc(offset_pc);
                print_source(line_entry->file->path, line_entry->line);
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

std::optional<dwarf::line_table::iterator>
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
                        return it;
                }

                return std::nullopt;
        }

        return std::nullopt;
}

PROCESS debugger_t::run_program(const char* str)
{
        return execl(str, str, nullptr);
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

#pragma endregion

#endif