#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include "response.hpp"
#include "key_value_store.hpp"
#include "errors.hpp"

// Base class for the Command interface
class Command
{
public:
    virtual ~Command() = default;
    virtual std::string execute() = 0;
};

#endif