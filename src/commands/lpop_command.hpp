#ifndef LPOP_COMMAND_HPP
#define LPOP_COMMAND_HPP

#include <vector>
#include "command.hpp"
#include "../key_value_store.hpp"
#include "../response.hpp"
#include "../errors.hpp"

class LpopCommand : public Command
{
private:
    std::string key;
    int num_values = 1;

public:
    explicit LpopCommand(const std::vector<std::string> &command_parts)
    {
        // Parse key and values
        if (command_parts.size() > 3)
            throw InvalidCommandStructureError("LPOP should have exactly 2 or 3 parts.");
        key = command_parts[1];

        // Parse num values
        try
        {
            if (command_parts.size() == 3)
                num_values = std::stoi(command_parts[2]);
        }
        catch (const std::invalid_argument &e)
        {
            throw InvalidCommandStructureError("LPOP's num_values should be an integer.");
        }
    }

    std::string execute() override
    {
        KeyValueStore &store = KeyValueStore::get_instance();

        // Get the length of the list corresponding to the key and return
        return Response::create_array_of_bulk_strings(store.pop_front_list_values(key, num_values));
    }
};

#endif