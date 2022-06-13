#pragma once
#include <string>
#include <unordered_map>
#include <optional>

#include "genericType.h"
#include "registers.h"

struct breakpoint_t
{

	breakpoint_t(){};

	breakpoint_t(const PROCESS process, std::uint64_t addr)
		: process(process)
		, addr(addr)
	{
	}

	void enable();
	void disable();

	bool is_enabled() const;
	std::uint64_t get_addr() const;

	PROCESS process = 0;
	std::uint64_t addr = 0;
	bool enabled = 0;
	std::uint8_t saved_data = 0;
};

struct Command
{
	enum Type
	{
		None = 0,
		Continue,
		Break,
		Register,
		Memory,
	};

	int type = Type::None;

	union
	{
		struct 
		{
			std::uint64_t address = 0;
		}breac;

		struct
		{
			enum RegisterCommand
			{
				None = 0,
				Dump,
				Read,
				Write,
			};

			int type = RegisterCommand::None;
			Reg reg = {};
			std::uint64_t value = 0;

		}regizter;

		struct
		{
			enum Action
			{
				None = 0,
				Read,
				Write,
			};

			std::uint64_t adress = 0;
			int action = Action::None;
			size_t value = 0;

		}memory;

	}arguments;

};

struct debugger_t
{
	struct CommandReturn
	{
		std::string error = "";
		std::uint64_t value = 0;
	};

	debugger_t(const std::string progName, const PROCESS process)
		: progName(progName)
		, process(process)
	{
	}

	CommandReturn handle_command(const std::string);
        std::uint64_t handle_command(Command command);
	void continue_execution();
	void set_breakpoint(const std::uint64_t addr);
	void dump_registers();
	std::uint64_t read_memory(const std::uint64_t addr);
	void write_memory(const std::uint64_t addr, const std::uint64_t value);
	std::uint64_t get_pc();
	void set_pc(const std::uint64_t);
	void step_over_breakpoint();
	void wait_for_signal();

	std::string progName = "";
	PROCESS process = 0;
	std::unordered_map<std::uint64_t, breakpoint_t> breakpoints = {};
};
