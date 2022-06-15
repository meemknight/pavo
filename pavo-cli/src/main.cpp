#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <fmt/core.h>
#include <genericType.h>
#include <vector>
#include <stringutils.h>
#include <registers.h>
#include "debugger.h"

void run(debugger_t& debugger)
{
		debugger.wait_for_signal();
		debugger.init_load_addr();

		fmt::print(stderr, "Load address: {:#018x}\n", debugger.load_address);

		while(true)
		{
				auto opt_line = LineEditing::get("pavo> ");
				if(!opt_line)
				{
						break;
				}

				std::string line = *opt_line;
				trim(line);

				if(line.empty())
				{
						continue;
				}

				debugger.handle_command(line);
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

		

		//fmt::print("Started debugging process {}\n", child);
		debugger_t debugger;
		debugger.start(prog_path);

		run(debugger);
}
