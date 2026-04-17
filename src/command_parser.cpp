#include <vector>
#include <string>

#include "buffer.hpp"
#include "command_parser.hpp"

std::vector<std::string> CommandParser::parse_command(Buffer &buf)
{
    // Resulting vector of command parts
    std::vector<std::string> parts;

    // Start parsing the command bytes at read_pos
    buf.reset_curr_pos();

    // If the first byte is not a *, we can raise an error
    if (buf.get_curr_char() != '*')
        throw std::domain_error("Invalid command bytes");
    buf.increment_curr_pos();

    // Parse the number words in the command using Horner's method
    unsigned char c;
    size_t num_words = 0;
    while (std::isdigit((c = buf.get_curr_char())))
    {
        num_words = num_words * 10 + (c - '0');
        buf.increment_curr_pos();
    }

    // curr_pos will be at \r now, so we can increment it by 2 to get to the $
    buf.increment_curr_pos(2);

    // Loop over the number of words and parse each word
    for (size_t i = 0; i < num_words; i++)
    {
        // If curr_pos is not a $ we can raise an error
        if (buf.get_curr_char() != '$')
            throw std::domain_error("Invalid command bytes");
        buf.increment_curr_pos();

        // Parse the number of chars in the current part in the same way as above
        size_t num_chars = 0;
        while (std::isdigit((c = buf.get_curr_char())))
        {
            num_chars = num_chars * 10 + (c - '0');
            buf.increment_curr_pos();
        }

        // curr_pos will be at \r now, so we can increment it by 2 to get to the $
        buf.increment_curr_pos(2);

        // Parse num_chars number of characters as the part in the range [curr_pos, curr_pos + num_chars)
        std::string part(reinterpret_cast<char *>(buf.get_pointer(buf.get_curr_pos())), num_chars);
        parts.push_back(part);

        // curr_pos has to be incremented by 'num_chars + 2' to account for the \r\n at the end of the part
        buf.increment_curr_pos(num_chars + 2);
    }

    // Convert the command (index 0 in the vector to lower case)
    std::transform(parts[0].begin(), parts[0].end(), parts[0].begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });

    // Update the read_pos cursor to curr_pos and return the command parts
    buf.set_read_pos(buf.get_curr_pos());
    return parts;
}