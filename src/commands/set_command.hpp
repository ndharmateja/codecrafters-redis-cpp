#ifndef SET_COMMAND_HPP
#define SET_COMMAND_HPP

#include <optional>
#include <vector>
#include <strings.h>
#include "command.hpp"
#include "../response.hpp"
#include "../errors.hpp"
#include "../key_value_store.hpp"

class SetCommand : public Command
{
private:
    std::string key;
    std::string value;
    std::optional<long long> expiry_in_ms;

public:
    explicit SetCommand(const std::vector<std::string> &command_parts)
    {
        // Parse key and value
        if (command_parts.size() != 3 && command_parts.size() != 5)
            throw InvalidCommandStructureError("SET should have exactly 3 or 5 parts.");
        key = command_parts[1];
        value = command_parts[2];

        // parse EX or PX args
        if (command_parts.size() == 3)
        {
            expiry_in_ms = std::nullopt;
            return;
        }

        // Only support EX or PX now
        if (!strcasecmp("EX", command_parts[3].c_str()) && !strcasecmp("PX", command_parts[3].c_str()))
            throw InvalidCommandStructureError("SET only supports EX/PX args.");

        // parse the number
        try
        {
            expiry_in_ms = std::stoll(command_parts[4]);
        }
        catch (const std::invalid_argument &e)
        {
            throw InvalidCommandStructureError("Arg for EX/PX should be a number.");
        }

        // If EX, then multiply the time by 1000 to get it in ms
        if (strcasecmp("EX", command_parts[3].c_str()))
            expiry_in_ms = expiry_in_ms.value() * 1000;
    }

    std::string execute() override
    {
        KeyValueStore &store = KeyValueStore::get_instance();
        store.set_value(key, value, expiry_in_ms);
        return Response::create_simple_string("OK");
    }
};

#endif