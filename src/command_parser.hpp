
#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <vector>
#include <string>
#include "client.hpp"
#include "buffer.hpp"

class CommandParser
{
public:
    /**
     * Parses the command bytes into a vector of command parts using the
     * RESP protocol assuming that the input bytes is an array of bulk strings.
     * It starts parsing from the read_pos of the buffer. Assumes that there is
     * atleast one valid command starting at the read_pos of the buffer.
     *
     * If input is bytes: *3\r\n$3\r\nSET\r\n$3\r\nfoo\r\n$3\r\nbar\r\n
     * output will be ["SET", "foo", "bar"]
     */
    static std::vector<std::string> parse_command(Buffer &buf);

    /**
     * Validates and returns if there is atleast one valid command in the
     * buffer starting at its read_pos.
     *
     * throws CommandParseError if the format is invalid
     */
    static bool has_one_command(Buffer &buf);
};

#endif