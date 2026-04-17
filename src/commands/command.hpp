#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>

// Base class for the Command interface
class Command
{
public:
    virtual ~Command() = default;
    virtual std::string execute() = 0;
};

#endif