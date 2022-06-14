#include "debugger.h"
#include "misc.h"
#include <genericType.h>
#include <vector>
#include "stringutils.h"
#include <fstream>

#ifdef PAVO_WIN32

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

void debugger_t::continue_execution()
{
	
}

void debugger_t::set_breakpoint(const std::uint64_t addr)
{
	
}

void debugger_t::dump_registers()
{
	
}

std::uint64_t debugger_t::read_memory(const std::uint64_t addr)
{
	return {};
}

void debugger_t::write_memory(const std::uint64_t addr, const std::uint64_t value)
{
}

std::uint64_t debugger_t::get_pc()
{
	return {};
}

void debugger_t::set_pc(const std::uint64_t val)
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

std::uint64_t debugger_t::offset_load_address(const std::uint64_t addr)
{
	return {};
}

std::optional<dwarf::die> debugger_t::get_function_from_pc(const std::uint64_t pc)
{
	return {};
}

std::optional<dwarf::line_table::iterator>
debugger_t::get_line_entry_from_pc(const std::uint64_t pc)
{
	return {};
}

PROCESS debugger_t::run_program(const char* str)
{
        return {};
}

#pragma endregion



#endif
