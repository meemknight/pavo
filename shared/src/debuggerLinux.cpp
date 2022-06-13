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

#ifdef PAVO_UNIX

#pragma region break point


static constexpr std::uint64_t BOTTOM_BYTE_MASK = 0xff;

void breakpoint_t::enable()
{
	auto data = ptrace(PTRACE_PEEKDATA, process, addr, nullptr);

	saved_data = static_cast<std::uint8_t>(data & BOTTOM_BYTE_MASK);

	std::uint64_t int3_op = 0xcc;
	std::uint64_t mask = ~BOTTOM_BYTE_MASK;
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


//todo error report
void debugger_t::handle_command(const std::string line)
{
	std::vector<std::string> args;
	split(args, line);

	const std::string command = args.front();
	if (command == "continue")
	{
		Command c = {};
		
		c.type = Command::Type::Continue;
		
		handle_command(c);
	}
	else if (command == "break")
	{
		if (args.size() < 2)
		{
			//errors
			//fmt::print(stderr, "Missing 'break' argument '0x<ADDRESS>'\n");
			return;
		}

		const std::string str_addr(args[1], 2);

		const auto opt_addr = u64_from_hex(str_addr);
		if (!opt_addr)
		{
			//fmt::print(stderr,
			//	"Expected address in hex format. Got: '{}' instead\n",
			//	str_addr);
			return;
		}

		Command c = {};
		c.type = Command::Type::Break;
		c.arguments.breac.address = opt_addr.value();

		handle_command(c);
	}
	else if (command == "register")
	{
		Command c = {};
		c.type = Command::Type::Register;

		if (args.size() == 2 && args[1] == "dump")
		{
			c.arguments.regizter.type = c.arguments.regizter.Dump;
			handle_command(c);
			return;
		}

		if (args.size() == 3 && args[1] == "read")
		{
			const auto reg = get_register_from_name(args[2]);
			if (!reg)
			{
				//fmt::print(stderr, "No such register: {}\n", args[2]);
				return;
			}

			c.arguments.regizter.type = c.arguments.regizter.Read;
			c.arguments.regizter.reg = *reg;

			handle_command(c);
			//fmt::print("{}\n", *get_register_value(pid, *reg));
			return;
		}

		if (args.size() == 4 && args[1] == "write")
		{
			const auto reg = get_register_from_name(args[2]);
			if (!reg)
			{
				//fmt::print(stderr, "No such register: {}\n", args[2]);
				return;
			}

			const std::string val_str = args[3];
			const auto opt_val = u64_from_hex(val_str);
			if (!opt_val)
			{
				//fmt::print(stderr,
				//	"Expected value in hex format. Got: '{}' instead\n",
				//	val_str);
				return;
			}

			c.arguments.regizter.type = c.arguments.regizter.Write;
			c.arguments.regizter.value = *opt_val;

			handle_command(c);
			return;
		}

		fmt::print(stderr, "Error. Available 'register' commands: dump, read <reg>, "
			"write <reg> <value>\n");
	}
	else if (command == "memory")
	{
		Command c = {};
		c.type = Command::Type::Memory;

		std::uint64_t addr = 0;
		if (args.size() > 2)
		{
			const std::string str_addr = args[2];
			const auto opt_addr = u64_from_hex(str_addr);
			if (!opt_addr)
			{
				//fmt::print(
				//	stderr,
				//	"Expected address in hex format. Got: '{}' instead\n",
				//	str_addr);
				return;
			}

			addr = *opt_addr;
		}

		c.arguments.memory.adress = addr;

		//todo check argument == "read"
		if (args.size() == 3)
		{
			c.arguments.memory.action = c.arguments.memory.Read;
			handle_command(c);
			return;
		}

		//todo check argument == "write"
		if (args.size() == 4)
		{
			c.arguments.memory.action = c.arguments.memory.Write;

			const std::string val_str = args[3];
			const auto opt_val = u64_from_hex(val_str);

			if (!opt_val)
			{
				//fmt::print(stderr,
				//	"Expected value in hex format. Got: '{}' instead\n",
				//	val_str);
				return;
			}

			c.arguments.memory.value = *opt_val;
			handle_command(c);

			return;
		}

		//fmt::print(stderr, "Error. Available 'memory' commands: read <addr>, "
		//	"write <addr> <value>\n");
		return;
	}
	else
	{
		//fmt::print(stderr, "Unknown command: '{}'\n", command);
		return;
	}
}


void debugger_t::handle_command(Command command)
{

	switch (command.type)
	{
		case Command::Type::Continue:
			continue_execution();
		break;

		case Command::Type::Break:
			set_breakpoint(command.arguments.breac.address);
		break;

		case Command::Type::Register:
			switch(command.arguments.regizter.type)
			{
				
				case command.arguments.regizter.Dump:
					dump_registers();
				break;

				case command.arguments.regizter.Read:
					fmt::print("{}\n", *get_register_value(process, command.arguments.regizter.reg));

				break;

				case command.arguments.regizter.Write:
				
					set_register_value(process, command.arguments.regizter.reg, command.arguments.regizter.value);

				break;

			}
		break;

		case Command::Type::Memory:
			
			switch(command.arguments.memory.action)
			{
				case command.arguments.memory.Read:
					fmt::print("{:#018x}\n", read_memory(command.arguments.memory.adress));
				break;

				case command.arguments.memory.Write:
					write_memory(command.arguments.memory.adress, command.arguments.memory.value);
				break;
			}

		break;

		default:
                        break;

	}

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
	for (const auto &reg_desc : register_descriptors)
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
	const std::uint64_t prev_location = get_pc() - 1;

	if (contains(breakpoints, prev_location))
	{
		auto &bp = breakpoints.at(prev_location);

		if (bp.is_enabled())
		{
			set_pc(prev_location);

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
}

#pragma endregion


#endif
