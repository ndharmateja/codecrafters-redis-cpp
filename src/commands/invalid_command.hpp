#ifndef INVALID_COMMAND_HPP
#define INVALID_COMMAND_HPP

#include <vector>
#include <string>
#include "command.hpp"

class InvalidCommand : public Command
{
public:
    explicit InvalidCommand(const std::vector<std::string> &command_parts) {}

    std::string execute() override { return Response::create_simple_error("Invalid/unsupported command"); }
};

#endif