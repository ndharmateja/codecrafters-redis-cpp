#ifndef PING_COMMAND_HPP
#define PING_COMMAND_HPP

#include <vector>
#include <string>
#include "../response.hpp"
#include "../errors.hpp"
#include "command.hpp"

class PingCommand : public Command
{
public:
    explicit PingCommand(const std::vector<std::string> &command_parts)
    {
        if (command_parts.size() != 1)
            throw InvalidCommandStructureError("PING command should have exactly 1 part.");
    }

    std::string execute() override { return Response::create_simple_string("PONG"); }
};

#endif