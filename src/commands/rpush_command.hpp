#ifndef RPUSH_COMMAND_HPP
#define RPUSH_COMMAND_HPP

#include <optional>
#include <vector>
#include <deque>
#include "command.hpp"
#include "../key_value_store.hpp"
#include "../response.hpp"
#include "../errors.hpp"

class RpushCommand : public Command
{
private:
    std::string key;
    std::deque<std::string> values;

public:
    explicit RpushCommand(const std::vector<std::string> &command_parts)
    {
        // Parse key and values
        if (command_parts.size() < 3)
            throw InvalidCommandStructureError("RPUSH should have atleast 3 parts.");
        key = command_parts[1];

        // Add all the values to the deque
        auto it = command_parts.begin() + 2;
        while (it != command_parts.end())
        {
            values.push_back(*it);
            it++;
        }
    }

    std::string execute() override
    {
        KeyValueStore &store = KeyValueStore::get_instance();

        // If value exists, we send that corresponding value otherwise
        // we send the null string back
        return Response::create_integer_string(store.push_back_list_values(key, values));
    }
};

#endif