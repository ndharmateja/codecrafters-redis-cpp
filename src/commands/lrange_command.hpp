#ifndef LRANGE_COMMAND_HPP
#define LRANGE_COMMAND_HPP

#include <optional>
#include <vector>
#include <strings.h>
#include "command.hpp"
#include "../response.hpp"
#include "../errors.hpp"
#include "../key_value_store.hpp"

class LrangeCommand : public Command
{
private:
    std::string key;
    int start;
    int stop;

public:
    explicit LrangeCommand(const std::vector<std::string> &command_parts)
    {
        // Parse key and value
        if (command_parts.size() != 4)
            throw InvalidCommandStructureError("LRANGE should have exactly 4 parts.");
        key = command_parts[1];

        try
        {
            start = std::stoi(command_parts[2]);
            stop = std::stoi(command_parts[3]);
        }
        catch (const std::invalid_argument &e)
        {
            throw InvalidCommandStructureError("start and stop args should be integers.");
        }
    }

    std::string execute() override
    {
        KeyValueStore &store = KeyValueStore::get_instance();
        return Response::create_array_of_bulk_strings(store.get_list_values(key, start, stop));
    }
};

#endif