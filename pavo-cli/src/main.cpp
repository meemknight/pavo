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

/* declarations */
class debugger_t;
class breakpoint_t;
/* declarations */

class breakpoint_t
{
public:
        breakpoint_t(const PID pid, std::intptr_t addr)
            : pid(pid)
            , addr(addr)
        {
        }

        void enable();
        void disable();

        bool is_enabled() const;
        std::intptr_t get_addr() const;

        static constexpr std::uint64_t BOTTOM_BYTE_MASK = 0xff;

private:
        PID pid;
        std::intptr_t addr;
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
        void set_breakpoint(const std::intptr_t addr);

private:
        std::string prog;
        PID pid;
        std::unordered_map<std::intptr_t, breakpoint_t> breakpoints;
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

std::intptr_t breakpoint_t::get_addr() const
{
        return addr;
}

void debugger_t::run()
{
        int wstatus;
        waitpid(pid, &wstatus, 0);

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
                long addr = 0;
                try
                {
                        addr = std::stol(str_addr, nullptr, 16);
                }
                catch(const std::invalid_argument& ex)
                {
                        fmt::print(stderr,
                                   "Expected address in hex format. Got: '{}' instead\n",
                                   str_addr);
                        return;
                }

                set_breakpoint(addr);
        }
        else
        {
                fmt::print(stderr, "Unknown command: '{}'\n", command);
        }
}

void debugger_t::continue_execution()
{
        ptrace(PTRACE_CONT, pid, nullptr, nullptr);

        int wstatus;
        waitpid(pid, &wstatus, 0);
}

void debugger_t::set_breakpoint(const std::intptr_t addr)
{
        fmt::print("Breakpoint set at address {:0X}\n", addr);

        breakpoint_t breakpoint{pid, addr};
        breakpoint.enable();
        breakpoints.insert(std::make_pair(addr, breakpoint));
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
