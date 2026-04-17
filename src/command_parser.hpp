
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
     * read_pos is the start of the bytes of the redis command
     *
     * If input is bytes: *3\r\n$3\r\nSET\r\n$3\r\nfoo\r\n$3\r\nbar\r\n
     * output will be ["SET", "foo", "bar"]
     *
     */
    static std::vector<std::string> parse_command(Buffer &buf);

    /**
     * Validates and returns if there is atleast one valid command in the
     * buffer starting at its currpos
     *
     * throws error if the format is invalid
     */
    static bool has_one_command(Buffer &buf) { return buf.get_write_pos() - buf.get_read_pos() >= 14; }
};

#endif