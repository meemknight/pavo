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
#include <registers.h>
#include "debugger.h"

void run(debugger_t& debugger)
{
        debugger.wait_for_signal();
        debugger.init_load_addr();

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

                debugger.handle_command(line);
                linenoiseHistoryAdd(line_buf);
                linenoiseFree(line_buf);
        }
}

int main(const int argc, const char* argv[])
{
        if(argc < 2)
        {
                fmt::print(stderr, "pavo: Program name not specified\n");
                return EXIT_FAILURE;
        }

        const std::string prog_path = argv[1];

        const PROCESS child = debugger_t::run_program(prog_path);
        if(child == EXIT_FAILURE)
        {
                return EXIT_FAILURE;
        }

        fmt::print("Started debugging process {}\n", child);
        debugger_t debugger(prog_path, child);
        run(debugger);
}
