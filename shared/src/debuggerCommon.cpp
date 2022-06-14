#include "debugger.h"
#include "stringutils.h"
#include "fmt/core.h"
#include "misc.h"
#include <vector>

debugger_t::debugger_t(const std::string progName, const PROCESS process)
    : progName(progName)
    , process(process)
{
        const auto fd = open(progName.c_str(), O_RDONLY);
        elf           = elf_wrapper::elf{elf_wrapper::create_mmap_loader(fd)};
        dwarf         = dwarf_wrapper::dwarf{dwarf_wrapper::elf::create_loader(elf)};
}

debugger_t::CommandReturn debugger_t::handle_command(const std::string line)
{
        std::vector<std::string> args;
        split(args, line);

        const std::string command = args.front();
        if(command == "continue")
        {
                Command c = {};

                c.type = Command::Type::Continue;

                return {"", handle_command(c)};
        }
        else if(command == "stepi")
        {
                Command c = {};
                c.type    = Command::Type::StepInstruction;
                return {"", handle_command(c)};
        }
        else if(command == "break")
        {
                if(args.size() < 2)
                {
                        return {fmt::format("Missing 'break' argument '0x<ADDRESS>'\n"), 0};
                }

                const std::string str_addr = args[1];
                const auto opt_addr        = u64_from_hex(str_addr);

                if(!opt_addr)
                {
                        return {
                            fmt::format("Expected address in hex format. Got: '{}' instead\n",
                                        str_addr),
                            0};
                }

                Command c             = {};
                c.type                = Command::Type::Break;
                c.args.b_args.address = opt_addr.value();

                return {"", handle_command(c)};
        }
        else if(command == "register")
        {
                Command c = {};
                c.type    = Command::Type::Register;

                if(args.size() == 2 && args[1] == "dump")
                {
                        c.args.r_args.type = c.args.r_args.Dump;
                        return {"", handle_command(c)};
                }

                if(args.size() == 3 && args[1] == "read")
                {
                        const auto reg = get_register_from_name(args[2]);
                        if(!reg)
                        {
                                return {fmt::format("No such register: {}\n", args[2]), 0};
                        }

                        c.args.r_args.type = c.args.r_args.Read;
                        c.args.r_args.reg  = *reg;

                        return {"", handle_command(c)};
                }

                if(args.size() == 4 && args[1] == "write")
                {
                        const auto reg = get_register_from_name(args[2]);
                        if(!reg)
                        {
                                return {fmt::format("No such register: {}\n", args[2]), 0};
                        }

                        const std::string val_str = args[3];
                        const auto opt_val        = u64_from_hex(val_str);
                        if(!opt_val)
                        {
                                return {
                                    fmt::format(
                                        "Expected value in hex format. Got: '{}' instead\n",
                                        val_str),
                                    0};
                        }

                        c.args.r_args.type  = c.args.r_args.Write;
                        c.args.r_args.reg   = *reg;
                        c.args.r_args.value = *opt_val;

                        return {"", handle_command(c)};
                }

                return {fmt::format("Error. Available 'register' commands: dump, read <reg>, "
                                    "write <reg> <value>\n"),
                        0};
        }
        else if(command == "memory")
        {
                Command c = {};
                c.type    = Command::Type::Memory;

                std::uint64_t addr = 0;
                if(args.size() > 2)
                {
                        const std::string str_addr = args[2];
                        const auto opt_addr        = u64_from_hex(str_addr);
                        if(!opt_addr)
                        {
                                const std::string err = fmt::format(
                                    "Expected address in hex format. Got: '{}' instead\n",
                                    str_addr);

                                return CommandReturn{err, 0};
                        }

                        addr = *opt_addr;
                }

                c.args.m_args.adress = addr;

                if(args.size() == 3 && args[1] == "read")
                {
                        c.args.m_args.type = c.args.m_args.Read;
                        return {"", handle_command(c)};
                }

                if(args.size() == 4 && args[1] == "write")
                {
                        c.args.m_args.type = c.args.m_args.Write;

                        const std::string val_str = args[3];
                        const auto opt_val        = u64_from_hex(val_str);

                        if(!opt_val)
                        {
                                return {
                                    fmt::format(
                                        "Expected value in hex format. Got: '{}' instead\n",
                                        val_str),
                                    0};
                        }

                        c.args.m_args.value = *opt_val;
                        return {"", handle_command(c)};
                }

                return {fmt::format("Error. Available 'memory' commands: read <addr>, "
                                    "write <addr> <value>\n"),
                        0};
        }
        else
        {
                return {fmt::format("Unknown command: '{}'\n", command), 0};
        }
}

std::uint64_t debugger_t::handle_command(Command command)
{
        switch(command.type)
        {
        case Command::Type::Continue:
        {
                continue_execution();
                break;
        }
        case Command::Type::StepInstruction:
        {
                single_step_instruction_check_br();
                auto line_entry = *get_line_entry_from_pc(get_pc());
                print_source(line_entry->file->path, line_entry->line, 2);
                break;
        }
        case Command::Type::Break:
        {
                set_breakpoint(command.args.b_args.address);
                break;
        }
        case Command::Type::Register:
        {
                switch(command.args.r_args.type)
                {
                case command.args.r_args.Dump:
                {
                        dump_registers();
                        break;
                }
                case command.args.r_args.Read:
                {
                        fmt::print("{}\n",
                                   *get_register_value(process, command.args.r_args.reg));

                        return *get_register_value(process, command.args.r_args.reg);
                }
                case command.args.r_args.Write:
                {
                        set_register_value(process, command.args.r_args.reg,
                                           command.args.r_args.value);

                        break;
                }
                }

                break;
        }
        case Command::Type::Memory:
        {
                switch(command.args.m_args.type)
                {
                case command.args.m_args.Read:
                {
                        fmt::print("{:#018x}\n", read_memory(command.args.m_args.adress));
                        return read_memory(command.args.m_args.adress);
                }
                case command.args.m_args.Write:
                {
                        write_memory(command.args.m_args.adress, command.args.m_args.value);
                        break;
                }
                }

                break;
        }
        default:
        {
                break;
        }
        }

        return 0;
}
