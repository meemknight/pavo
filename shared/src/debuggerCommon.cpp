#include "debugger.h"
#include "stringutils.h"
#include "fmt/core.h"
#include "misc.h"
#include <vector>

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
        else if(command == "break")
        {
                if(args.size() < 2)
                {
                        return {fmt::format("Missing 'break' argument '0x<ADDRESS>'\n"), 0};
                }

                const std::string str_addr(args[1], 2);

                const auto opt_addr = u64_from_hex(str_addr);
                if(!opt_addr)
                {
                        return {
                            fmt::format("Expected address in hex format. Got: '{}' instead\n",
                                        str_addr),
                            0};
                }

                Command c                 = {};
                c.type                    = Command::Type::Break;
                c.arguments.breac.address = opt_addr.value();

                return {"", handle_command(c)};
        }
        else if(command == "register")
        {
                Command c = {};
                c.type    = Command::Type::Register;

                if(args.size() == 2 && args[1] == "dump")
                {
                        c.arguments.regizter.type = c.arguments.regizter.Dump;
                        return {"", handle_command(c)};
                }

                if(args.size() == 3 && args[1] == "read")
                {
                        const auto reg = get_register_from_name(args[2]);
                        if(!reg)
                        {
                                return {fmt::format("No such register: {}\n", args[2]), 0};
                        }

                        c.arguments.regizter.type = c.arguments.regizter.Read;
                        c.arguments.regizter.reg  = *reg;

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

                        c.arguments.regizter.type  = c.arguments.regizter.Write;
                        c.arguments.regizter.value = *opt_val;

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

                c.arguments.memory.adress = addr;

                if(args.size() == 3 && args[1] == "read")
                {
                        c.arguments.memory.action = c.arguments.memory.Read;
                        return {"", handle_command(c)};
                }

                if(args.size() == 4 && args[1] == "write")
                {
                        c.arguments.memory.action = c.arguments.memory.Write;

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

                        c.arguments.memory.value = *opt_val;
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
                        fmt::print("{}\n", *get_register_value(
                                               process, command.arguments.regizter.reg));

                        return *get_register_value(process, command.arguments.regizter.reg);

                case command.arguments.regizter.Write:

                        set_register_value(process, command.arguments.regizter.reg,
                                           command.arguments.regizter.value);

                        break;
                }
                break;

        case Command::Type::Memory:

                switch(command.arguments.memory.action)
                {
                case command.arguments.memory.Read:
                        fmt::print("{:#018x}\n", read_memory(command.arguments.memory.adress));
                        return read_memory(command.arguments.memory.adress);

                case command.arguments.memory.Write:
                        write_memory(command.arguments.memory.adress,
                                     command.arguments.memory.value);
                        break;
                }

                break;

        default:
                break;
        }

        return 0;
}
