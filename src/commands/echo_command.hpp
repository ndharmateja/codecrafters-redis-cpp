#ifndef ECHO_COMMAND_HPP
#define ECHO_COMMAND_HPP

#include <vector>
#include <string>
#include "command.hpp"

class EchoCommand : public Command
{
private:
    std::string msg;

public:
    explicit EchoCommand(const std::vector<std::string> &command_parts)
    {
        if (command_parts.size() != 2)
            throw InvalidCommandStructureError("Echo command should have exactly 2 parts.");

        // Assign message
        msg = command_parts[1];
    }

    std::string execute() override { return Response::create_bulk_string(msg); }
};

#endif