#ifndef PING_COMMAND_HPP
#define PING_COMMAND_HPP

#include <vector>
#include <string>
#include "command.hpp"

class PingCommand : public Command
{
public:
    explicit PingCommand(const std::vector<std::string> &command_parts)
    {
        if (command_parts.size() != 2)
            throw InvalidCommandStructureError("Echo command should have exactly 2 parts.");
    }

    std::string execute() override { return Response::create_simple_string("PONG"); }
};

#endif