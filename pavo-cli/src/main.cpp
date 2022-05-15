#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fmt/core.h>
#include <linenoise.h>
#include <genericType.h>
#include <vector>
#include <stringutils.h>
#include <sys/personality.h>
#include <sys/user.h>
#include <optional>
#include <misc.h>

/* register stuff */
enum class reg {
        rax,
        rbx,
        rcx,
        rdx,
        rdi,
        rsi,
        rbp,
        rsp,
        r8,
        r9,
        r10,
        r11,
        r12,
        r13,
        r14,
        r15,
        rip,
        rflags,
        cs,
        orig_rax,
        fs_base,
        gs_base,
        fs,
        gs,
        ss,
        ds,
        es
};

static constexpr std::size_t n_registers = 27;

struct reg_descriptor
{
        reg r;
        int dwarf_r;
        std::string name;
};

static const std::array<reg_descriptor, n_registers> register_descriptors{{
    {reg::r15, 15, "r15"},
    {reg::r14, 14, "r14"},
    {reg::r13, 13, "r13"},
    {reg::r12, 12, "r12"},
    {reg::rbp, 6, "rbp"},
    {reg::rbx, 3, "rbx"},
    {reg::r11, 11, "r11"},
    {reg::r10, 10, "r10"},
    {reg::r9, 9, "r9"},
    {reg::r8, 8, "r8"},
    {reg::rax, 0, "rax"},
    {reg::rcx, 2, "rcx"},
    {reg::rdx, 1, "rdx"},
    {reg::rsi, 4, "rsi"},
    {reg::rdi, 5, "rdi"},
    {reg::orig_rax, -1, "orig_rax"},
    {reg::rip, -1, "rip"},
    {reg::cs, 51, "cs"},
    {reg::rflags, 49, "eflags"},
    {reg::rsp, 7, "rsp"},
    {reg::ss, 52, "ss"},
    {reg::fs_base, 58, "fs_base"},
    {reg::gs_base, 59, "gs_base"},
    {reg::ds, 53, "ds"},
    {reg::es, 50, "es"},
    {reg::fs, 54, "fs"},
    {reg::gs, 55, "gs"},
}};

static std::optional<std::uint64_t> get_register_value(const PID pid, const reg r)
{
        const auto reg_it =
            std::find_if(register_descriptors.begin(), register_descriptors.end(),
                         [&r](const auto& reg_desc)
                         {
                                 return reg_desc.r == r;
                         });

        if(reg_it == register_descriptors.end())
        {
                return std::nullopt;
        }

        const auto reg_idx = reg_it - register_descriptors.begin();

        user_regs_struct regs;
        ptrace(PTRACE_GETREGS, pid, nullptr, &regs);

        return *(reinterpret_cast<std::uint64_t*>(&regs) + reg_idx);
}

static int set_register_value(const PID pid, const reg r, const std::uint64_t value)
{
        const auto reg_it =
            std::find_if(register_descriptors.begin(), register_descriptors.end(),
                         [&r](const auto& reg_desc)
                         {
                                 return reg_desc.r == r;
                         });

        if(reg_it == register_descriptors.end())
        {
                return -1;
        }

        const auto reg_idx = reg_it - register_descriptors.begin();

        user_regs_struct regs;
        ptrace(PTRACE_GETREGS, pid, nullptr, &regs);

        *(reinterpret_cast<std::uint64_t*>(&regs) + reg_idx) = value;

        ptrace(PTRACE_SETREGS, pid, nullptr, &regs);
        return 0;
}

static std::optional<std::uint64_t>
get_register_value_from_dwarf_register(const PID pid, const int reg_dwarf_r)
{
        const auto reg_it =
            std::find_if(register_descriptors.begin(), register_descriptors.end(),
                         [&reg_dwarf_r](const auto& reg_desc)
                         {
                                 return reg_desc.dwarf_r == reg_dwarf_r;
                         });

        if(reg_it == register_descriptors.end())
        {
                return std::nullopt;
        }

        return get_register_value(pid, reg_it->r);
}

static std::string get_register_name(const reg r)
{
        const auto reg_it =
            std::find_if(register_descriptors.begin(), register_descriptors.end(),
                         [&r](const auto& reg_desc)
                         {
                                 return reg_desc.r == r;
                         });

        if(reg_it == register_descriptors.end())
        {
                throw std::out_of_range{"Unknown register"};
        }

        return reg_it->name;
}

static std::optional<reg> get_register_from_name(const std::string& name)
{
        const auto reg_it =
            std::find_if(register_descriptors.begin(), register_descriptors.end(),
                         [&name](const auto& reg_desc)
                         {
                                 return reg_desc.name == name;
                         });

        if(reg_it == register_descriptors.end())
        {
                return std::nullopt;
        }

        return reg_it->r;
}
/* register stuff */

/* declarations */
class debugger_t;
class breakpoint_t;
/* declarations */

class breakpoint_t
{
public:
        breakpoint_t(const PID pid, std::uint64_t addr)
            : pid(pid)
            , addr(addr)
        {
        }

        void enable();
        void disable();

        bool is_enabled() const;
        std::uint64_t get_addr() const;

        static constexpr std::uint64_t BOTTOM_BYTE_MASK = 0xff;

private:
        PID pid;
        std::uint64_t addr;
        bool enabled;
        std::uint8_t saved_data;
};

class debugger_t
{
public:
        debugger_t(const std::string prog, const PID pid)
            : prog(prog)
            , pid(pid)
        {
        }

        void run();
        void handle_command(const std::string);
        void continue_execution();
        void set_breakpoint(const std::uint64_t addr);
        void dump_registers();
        std::uint64_t read_memory(const std::uint64_t addr);
        void write_memory(const std::uint64_t addr, const std::uint64_t value);
        std::uint64_t get_pc();
        void set_pc(const std::uint64_t);
        void step_over_breakpoint();
        void wait_for_signal();

private:
        std::string prog;
        PID pid;
        std::unordered_map<std::uint64_t, breakpoint_t> breakpoints;
};

void breakpoint_t::enable()
{
        auto data = ptrace(PTRACE_PEEKDATA, pid, addr, nullptr);

        saved_data = static_cast<std::uint8_t>(data & BOTTOM_BYTE_MASK);

        std::uint64_t int3_op = 0xcc;
        std::uint64_t mask = ~BOTTOM_BYTE_MASK;
        std::uint64_t new_data = ((data & mask) | int3_op);
        ptrace(PTRACE_POKEDATA, pid, addr, new_data);

        enabled = true;
}

void breakpoint_t::disable()
{
        auto data = ptrace(PTRACE_PEEKDATA, pid, addr, nullptr);

        std::uint64_t restored_data = ((data & (~BOTTOM_BYTE_MASK)) | saved_data);
        ptrace(PTRACE_POKEDATA, pid, addr, restored_data);

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

void debugger_t::run()
{
        wait_for_signal();

        char* line_buf = nullptr;
        while(true)
        {
                line_buf = linenoise("pavo> ");
                if(line_buf == nullptr)
                {
                        break;
                }

                std::string line = line_buf;
                trim(line);

                if(line.empty())
                {
                        continue;
                }

                handle_command(line);
                linenoiseHistoryAdd(line_buf);
                linenoiseFree(line_buf);
        }
}

void debugger_t::handle_command(const std::string line)
{
        std::vector<std::string> args;
        split(args, line);

        const std::string command = args.front();
        if(command == "continue")
        {
                continue_execution();
        }
        else if(command == "break")
        {
                if(args.size() < 2)
                {
                        fmt::print(stderr, "Missing 'break' argument '0x<ADDRESS>'\n");
                        return;
                }

                const std::string str_addr(args[1], 2);

                const auto opt_addr = u64_from_hex(str_addr);
                if(!opt_addr)
                {
                        fmt::print(stderr,
                                   "Expected address in hex format. Got: '{}' instead\n",
                                   str_addr);
                        return;
                }

                set_breakpoint(*opt_addr);
        }
        else if(command == "register")
        {
                if(args.size() == 2 && args[1] == "dump")
                {
                        dump_registers();
                        return;
                }

                if(args.size() == 3 && args[1] == "read")
                {
                        const auto reg = get_register_from_name(args[2]);
                        if(!reg)
                        {
                                fmt::print(stderr, "No such register: {}\n", args[2]);
                                return;
                        }

                        fmt::print("{}\n", *get_register_value(pid, *reg));
                        return;
                }

                if(args.size() == 4 && args[1] == "write")
                {
                        const auto reg = get_register_from_name(args[2]);
                        if(!reg)
                        {
                                fmt::print(stderr, "No such register: {}\n", args[2]);
                                return;
                        }

                        const std::string val_str = args[3];
                        const auto opt_val = u64_from_hex(val_str);
                        if(!opt_val)
                        {
                                fmt::print(stderr,
                                           "Expected value in hex format. Got: '{}' instead\n",
                                           val_str);
                                return;
                        }

                        set_register_value(pid, *reg, *opt_val);
                        return;
                }

                fmt::print(stderr, "Error. Available 'register' commands: dump, read <reg>, "
                                   "write <reg> <value>\n");
        }
        else if(command == "memory")
        {
                std::uint64_t addr = 0;
                if(args.size() > 2)
                {
                        const std::string str_addr = args[2];
                        const auto opt_addr = u64_from_hex(str_addr);
                        if(!opt_addr)
                        {
                                fmt::print(
                                    stderr,
                                    "Expected address in hex format. Got: '{}' instead\n",
                                    str_addr);
                                return;
                        }

                        addr = *opt_addr;
                }

                if(args.size() == 3)
                {
                        fmt::print("{:#018x}\n", read_memory(addr));
                        return;
                }

                if(args.size() == 4)
                {
                        const std::string val_str = args[3];
                        const auto opt_val = u64_from_hex(val_str);

                        if(!opt_val)
                        {
                                fmt::print(stderr,
                                           "Expected value in hex format. Got: '{}' instead\n",
                                           val_str);
                                return;
                        }

                        write_memory(addr, *opt_val);
                        return;
                }

                fmt::print(stderr, "Error. Available 'memory' commands: read <addr>, "
                                   "write <addr> <value>\n");
        }
        else
        {
                fmt::print(stderr, "Unknown command: '{}'\n", command);
        }
}

void debugger_t::continue_execution()
{
        step_over_breakpoint();
        ptrace(PTRACE_CONT, pid, nullptr, nullptr);
        wait_for_signal();
}

void debugger_t::set_breakpoint(const std::uint64_t addr)
{
        fmt::print("Breakpoint set at address {:#018x}\n", addr);

        breakpoint_t breakpoint{pid, addr};
        breakpoint.enable();
        breakpoints.insert(std::make_pair(addr, breakpoint));
}

void debugger_t::dump_registers()
{
        for(const auto& reg_desc : register_descriptors)
        {
                fmt::print("{} {:#018x}\n", reg_desc.name,
                           *get_register_value(pid, reg_desc.r));
        }
}

std::uint64_t debugger_t::read_memory(const std::uint64_t addr)
{
        return ptrace(PTRACE_PEEKDATA, pid, addr, nullptr);
}

void debugger_t::write_memory(const std::uint64_t addr, const std::uint64_t value)
{
        ptrace(PTRACE_POKEDATA, pid, addr, value);
}

std::uint64_t debugger_t::get_pc()
{
        return *get_register_value(pid, reg::rip);
}

void debugger_t::set_pc(const std::uint64_t val)
{
        set_register_value(pid, reg::rip, val);
}

void debugger_t::step_over_breakpoint()
{
        const std::uint64_t prev_location = get_pc() - 1;

        if(contains(breakpoints, prev_location))
        {
                auto& bp = breakpoints.at(prev_location);

                if(bp.is_enabled())
                {
                        set_pc(prev_location);

                        bp.disable();
                        ptrace(PTRACE_SINGLESTEP, pid, nullptr, nullptr);
                        wait_for_signal();
                        bp.enable();
                }
        }
}

void debugger_t::wait_for_signal()
{
        int wait_status;
        waitpid(pid, &wait_status, 0);
}

int main(const int argc, const char* argv[])
{
        if(argc < 2)
        {
                fmt::print(stderr, "pavo: Program name not specified\n");
                return EXIT_FAILURE;
        }

        const std::string prog_path = argv[1];

        const PID child_pid = fork();
        if(child_pid < 0)
        {
                perror("fork");
                return EXIT_FAILURE;
        }

        if(child_pid == 0)
        {
                personality(ADDR_NO_RANDOMIZE);
                const auto ret = ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
                if(ret < 0)
                {
                        perror("ptrace");
                        return EXIT_FAILURE;
                }

                const char* prog_str = prog_path.data();
                execl(prog_str, prog_str, nullptr);

                perror("execl");
                return EXIT_FAILURE;
        }
        else
        {
                fmt::print("Started debugging process {}\n", child_pid);
                debugger_t debugger(prog_path, child_pid);
                debugger.run();
        }
}
