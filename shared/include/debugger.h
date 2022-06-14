#pragma once
#include <string>
#include <unordered_map>
#include <optional>
#include <fcntl.h>
#include <signal.h>

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

		PROCESS process         = 0;
		std::uint64_t addr      = 0;
		bool enabled            = 0;
		std::uint8_t saved_data = 0;
};

struct Command
{
		enum Type {
				None = 0,
				Continue,
				StepInstruction,
				Break,
				Register,
				Memory,
		};

		struct Break_args
		{
				std::uint64_t address = 0;
		};

		struct Register_args
		{
				enum Action {
						None = 0,
						Dump,
						Read,
						Write,
				};

				int type            = Action::None;
				Reg reg             = {};
				std::uint64_t value = 0;
		};

		struct Memory_args
		{
				enum Action {
						None = 0,
						Read,
						Write,
				};

				int type             = Action::None;
				std::uint64_t adress = 0;
				std::uint64_t value  = 0;
		};

		int type = Type::None;
		union
		{
				Break_args b_args;
				Register_args r_args;
				Memory_args m_args;
		} args;
};

struct debugger_t
{
		struct CommandReturn
		{
				std::string error   = "";
				std::uint64_t value = 0;
		};

		debugger_t() {};

		//debugger_t(const std::string progName, const PROCESS process)
		//	: progName(progName)
		//	, process(process)
		//{
		//		const auto fd = open(progName.c_str(), O_RDONLY);
		//		elf           = elf_wrapper::elf{elf_wrapper::create_mmap_loader(fd)};
		//		dwarf         = dwarf_wrapper::dwarf{dwarf_wrapper::elf::create_loader(elf)};
		//}

		std::string start(const std::string progName);


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
		void init_load_addr();
		std::uint64_t offset_load_address(const std::uint64_t);
		std::optional<dwarf_wrapper::die> get_function_from_pc(const std::uint64_t);
		std::optional<dwarf_wrapper::line_table::iterator>
		get_line_entry_from_pc(const std::uint64_t);
		void print_source(const std::string&, const unsigned line, const unsigned context = 2);
		siginfo_t get_signal_info();
		void handle_sigtrap(siginfo_t);
		void single_step_instruction();
		void single_step_instruction_check_br();

		static PROCESS run_program(const std::string& path);

		std::string progName;
		PROCESS process;
		std::unordered_map<std::uint64_t, breakpoint_t> breakpoints;
		dwarf_wrapper::dwarf dwarf;
		elf_wrapper::elf elf;
		std::uint64_t load_address;
};
