#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <vector>
#include <string>
#include "client.hpp"
#include "buffer.hpp"

class CommandParser
{
    /**
     * This parser parses a Redis command which is an array of bulk strings. If the full command
     * is not present in the buffer then client will receive more data.
     *
     * Throws a parsing error if the command is not valid.
     */
    static std::vector<std::string> parse_one_command(Client &client)
    {
    }
};

#endif