#include <iostream>
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

/* declarations */
class debugger_t;
class breakpoint_t;
/* declarations */

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

private:
        std::string prog;
        PID pid;
};

class breakpoint_t
{
public:
private:
};

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
