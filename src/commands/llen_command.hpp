#ifndef LLEN_COMMAND_HPP
#define LLEN_COMMAND_HPP

#include <vector>
#include "command.hpp"
#include "../key_value_store.hpp"
#include "../response.hpp"
#include "../errors.hpp"

class LlenCommand : public Command
{
private:
    std::string key;

public:
    explicit LlenCommand(const std::vector<std::string> &command_parts)
    {
        // Parse key and values
        if (command_parts.size() != 2)
            throw InvalidCommandStructureError("LLEN should have exactly 2 parts.");
        key = command_parts[1];
    }

    std::string execute() override
    {
        KeyValueStore &store = KeyValueStore::get_instance();

        // Get the length of the list corresponding to the key and return
        return Response::create_integer_string(store.get_list_length(key));
    }
};

#endif