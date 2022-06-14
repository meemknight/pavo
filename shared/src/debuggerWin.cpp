#include "debugger.h"
#include "misc.h"
#include <genericType.h>
#include <vector>
#include "stringutils.h"
#include <fstream>

#ifdef PAVO_WIN32

namespace dwarf = dwarf_wrapper;

#pragma region break point

void breakpoint_t::enable()
{
}

void breakpoint_t::disable()
{
}

bool breakpoint_t::is_enabled() const
{
		return 0;
}

std::uint64_t breakpoint_t::get_addr() const
{
		return 0;
}

#pragma endregion

#pragma region debugger

bool debugger_t::start(const std::string progName)
{
	const PROCESS child = debugger_t::run_program(progName);
	if (child == 0)
	{
		return 0;
	}

	this->progName = progName;
	this->process = child;
	return 1;
}

void debugger_t::continue_execution()
{
}

void debugger_t::set_breakpoint(const std::uint64_t addr)
{
		return;
}

void debugger_t::dump_registers()
{
		return;
}
std::uint64_t debugger_t::read_memory(const std::uint64_t addr)
{
		return {};
}

void debugger_t::write_memory(const std::uint64_t addr, const std::uint64_t value)
{
		return;
}

std::uint64_t debugger_t::get_pc()
{
		return {};
}

void debugger_t::set_pc(const std::uint64_t)
{
}

void debugger_t::step_over_breakpoint()
{
}

void debugger_t::wait_for_signal()
{
}

void debugger_t::init_load_addr()
{
}

std::uint64_t debugger_t::offset_load_address(const std::uint64_t)
{
		return {};
}

std::optional<dwarf_wrapper::die> debugger_t::get_function_from_pc(const std::uint64_t)
{
		return dwarf_wrapper::die();
}

std::optional<dwarf_wrapper::line_table>
debugger_t::get_line_entry_from_pc(const std::uint64_t)
{
	//return dwarf_wrapper::line_table::iterator(1);
	return dwarf_wrapper::line_table{};
}

void debugger_t::print_source(const std::string&, const unsigned line, const unsigned context)
{
}

SINFO debugger_t::get_signal_info()
{
		return {};
}

void debugger_t::handle_sigtrap(SINFO)
{
}

void debugger_t::single_step_instruction()
{
}

void debugger_t::single_step_instruction_check_br()
{
}

PROCESS debugger_t::run_program(const std::string &path)
{
	//todo implement
	return HANDLE(1);
}



#pragma endregion

#endif
