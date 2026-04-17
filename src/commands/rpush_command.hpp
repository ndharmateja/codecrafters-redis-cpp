#ifndef RPUSH_COMMAND_HPP
#define RPUSH_COMMAND_HPP

#include <optional>
#include <vector>
#include "command.hpp"
#include "../key_value_store.hpp"
#include "../response.hpp"
#include "../errors.hpp"

class RpushCommand : public Command
{
private:
    std::string key;
    std::string value;

public:
    explicit RpushCommand(const std::vector<std::string> &command_parts)
    {
        // Parse key and value
        if (command_parts.size() != 3)
            throw InvalidCommandStructureError("RPUSH should have exactly 3 parts.");
        key = command_parts[1];
        value = command_parts[2];
    }

    std::string execute() override
    {
        KeyValueStore &store = KeyValueStore::get_instance();

        // If value exists, we send that corresponding value otherwise
        // we send the null string back
        return Response::create_integer_string(store.push_back_list_value(key, value));
    }
};

#endif