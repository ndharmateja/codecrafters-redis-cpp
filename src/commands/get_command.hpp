#ifndef GET_COMMAND_HPP
#define GET_COMMAND_HPP

#include <optional>
#include <vector>
#include "command.hpp"
#include "../key_value_store.hpp"
#include "../response.hpp"
#include "../errors.hpp"

class GetCommand : public Command
{
private:
    std::string key;

public:
    explicit GetCommand(const std::vector<std::string> &command_parts)
    {
        // Parse key and value
        if (command_parts.size() != 2)
            throw InvalidCommandStructureError("GET should have exactly 2 parts.");
        key = command_parts[1];
    }

    std::string execute() override
    {
        KeyValueStore &store = KeyValueStore::get_instance();
        std::optional<std::string> result = store.get_value(key);

        // If value exists, we send that corresponding value otherwise
        // we send the null string back
        return result.has_value()
                   ? Response::create_bulk_string(result.value())
                   : Response::create_null_bulk_string();
    }
};

#endif