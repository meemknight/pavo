#include "registers.h"
#include <algorithm>

#ifdef PAVO_UNIX
#include <sys/ptrace.h>
#include <sys/user.h>



std::optional<std::uint64_t> get_register_value(const PROCESS process, const Reg r)
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
		ptrace(PTRACE_GETREGS, process, nullptr, &regs);

		return *(reinterpret_cast<std::uint64_t*>(&regs) + reg_idx);
}

int set_register_value(const PROCESS process, const Reg r, const std::uint64_t value)
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
		ptrace(PTRACE_GETREGS, process, nullptr, &regs);

		*(reinterpret_cast<std::uint64_t*>(&regs) + reg_idx) = value;

		ptrace(PTRACE_SETREGS, process, nullptr, &regs);
		return 0;
}



std::optional<std::uint64_t> get_register_value_from_dwarf_register(const PROCESS process,
																	const int reg_dwarf_r)
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

		return get_register_value(process, reg_it->r);
}

std::optional<std::string> get_register_name(const Reg r)
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

		return reg_it->name;
}

std::optional<Reg> get_register_from_name(const std::string& name)
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

#endif


#ifdef PAVO_WIN32

std::optional<std::uint64_t> get_register_value(const PROCESS process, const Reg r)
{
	return {};

}

int set_register_value(const PROCESS process, const Reg r, const std::uint64_t value)
{
	return {};

}


std::optional<std::uint64_t> get_register_value_from_dwarf_register(const PROCESS process,
	const int reg_dwarf_r)
{
	return {};

}

std::optional<std::string> get_register_name(const Reg r)
{
	return {};

}

std::optional<Reg> get_register_from_name(const std::string &name)
{
	return {};
}


#endif
