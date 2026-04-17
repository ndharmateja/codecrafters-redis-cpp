#ifndef COMMAND_FACTORY_HPP
#define COMMAND_FACTORY_HPP

#include <vector>
#include <string>
#include <memory>
#include "command.hpp"
#include "ping_command.hpp"
#include "set_command.hpp"
#include "get_command.hpp"
#include "invalid_command.hpp"
#include "echo_command.hpp"

class CommandFactory
{
    static std::unique_ptr<Command> create_command(const std::vector<std::string> &command_parts)
    {
        // Handle PING command
        if (command_parts.front() == "ping")
            return std::make_unique<PingCommand>();

        // Handle ECHO command with one argument
        // Send response back as a RESP bulk string
        else if (command_parts.front() == "echo")
            return std::make_unique<EchoCommand>(command_parts);

        // Handle SET command
        else if (command_parts.front() == "set")
            return std::make_unique<SetCommand>(command_parts);

        // Handle GET command
        else if (command_parts.front() == "get")
            return std::make_unique<GetCommand>(command_parts);

        // All other commands are unsupported
        else
            return std::make_unique<InvalidCommand>();
    }
};

#endif