#ifndef COMMAND_FACTORY_HPP
#define COMMAND_FACTORY_HPP

#include <vector>
#include <string>
#include <memory>
#include "command.hpp"
#include "ping_command.hpp"
#include "set_command.hpp"
#include "get_command.hpp"
#include "rpush_command.hpp"
#include "lpush_command.hpp"
#include "lpop_command.hpp"
#include "lrange_command.hpp"
#include "llen_command.hpp"
#include "invalid_command.hpp"
#include "echo_command.hpp"

class CommandFactory
{
public:
    static std::unique_ptr<Command> create_command(const std::vector<std::string> &command_parts)
    {
        // Handle PING command
        if (command_parts.front() == "ping")
            return std::make_unique<PingCommand>(command_parts);

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

        // Handle RPUSH command
        else if (command_parts.front() == "rpush")
            return std::make_unique<RpushCommand>(command_parts);

        // Handle LPUSH command
        else if (command_parts.front() == "lpush")
            return std::make_unique<LpushCommand>(command_parts);

        // Handle LPOP command
        else if (command_parts.front() == "lpop")
            return std::make_unique<LpopCommand>(command_parts);

        // Handle LRANGE command
        else if (command_parts.front() == "lrange")
            return std::make_unique<LrangeCommand>(command_parts);

        // Handle LLEN command
        else if (command_parts.front() == "llen")
            return std::make_unique<LlenCommand>(command_parts);

        // All other commands are unsupported
        else
            return std::make_unique<InvalidCommand>();
    }
};

#endif