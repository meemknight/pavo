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

        std::uint64_t int3_op  = 0xcc;
        std::uint64_t mask     = ~BOTTOM_BYTE_MASK;
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
        for(const auto& reg_desc : register_descriptors)
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

        if(contains(breakpoints, prev_location))
        {
                auto& bp = breakpoints.at(prev_location);

                if(bp.is_enabled())
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
