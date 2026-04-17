#ifndef SET_COMMAND_HPP
#define SET_COMMAND_HPP

#include <optional>
#include <vector>
#include "command.hpp"

class SetCommand : public Command
{
private:
    std::string key;
    std::string value;
    std::optional<std::chrono::steady_clock::time_point> expiry_time;

public:
    explicit SetCommand(const std::vector<std::string> &command_parts)
    {
        // Parse key and value
        if (command_parts.size() < 3)
            throw InvalidCommandStructureError("SET should have atleast 3 parts.");
        key = command_parts[1];
        value = command_parts[2];

        // parse EX or PX args
        if (command_parts.size() <= 4)
        {
            expiry_time = std::nullopt;
            return;
        }

        // Only support EX or PX now
        if (!strcasecmp("EX", command_parts[3].c_str()) || !strcasecmp("PX", command_parts[3].c_str()))
            throw InvalidCommandStructureError("SET only supports EX/PX args.");

        // parse the number
        long long expiry_in_ms;
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
            expiry_in_ms *= 1000;

        // Store the expiry time as a timestamp
        expiry_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(expiry_in_ms);
    }

    std::string execute() override
    {
        KeyValueStore &store = KeyValueStore::get_instance();
        store.set_value(key, value);
        return Response::create_simple_string("OK");
    }
};

#endif