#include <vector>
#include <string>

#include "buffer.hpp"
#include "command_parser.hpp"
#include "errors.hpp"

static int validate_is_digit(Buffer &buf, std::string message = "Missing digit.")
{
    unsigned char c;
    if (!std::isdigit((c = buf.get_curr_char())))
        throw CommandParseError(message);
    buf.increment_curr_pos();
    return c - '0';
}

static void validate_char(Buffer &buf, unsigned char c, std::string message = "Invalid character.")
{
    if (buf.get_curr_char() != c)
        throw CommandParseError(message);
    buf.increment_curr_pos();
}

static void validate_crlf(Buffer &buf, std::string message = "Missing \\r\\n.")
{
    validate_char(buf, '\r', message);
    validate_char(buf, '\n', message);
}

bool CommandParser::has_one_command(Buffer &buf)
{
    try
    {
        // If first character is not * then we throw error
        buf.reset_curr_pos();
        validate_char(buf, '*', "Missing '*' at the start.");

        // Parse the number of words using Horner's method
        // If the next char is not a digit, it is invalid
        unsigned char c;
        int num_words = validate_is_digit(buf, "Missing number after '*'.");

        // Parse all the digits
        while (std::isdigit((c = buf.get_curr_char())))
        {
            num_words = (num_words * 10) + (c - '0');
            buf.increment_curr_pos();
        }

        // The next two chars have to be \r and \n
        validate_crlf(buf, "Missing \\r\\n after the number of words.");

        // No we should have num_words number of parts of the form
        // $<num_chars>\r\n<word>\r\n
        for (size_t i = 0; i < num_words; i++)
        {
            // The first char should be '$'
            validate_char(buf, '$', "Missing '$'.");

            // Parse the number of chars in the word using Horner's method
            // If the next char is not a digit, it is invalid
            int num_chars = validate_is_digit(buf, "Missing number after '$'.");

            // Parse all the digits
            while (std::isdigit((c = buf.get_curr_char())))
            {
                num_chars = (num_chars * 10) + (c - '0');
                buf.increment_curr_pos();
            }

            // The next two chars have to be \r and \n
            validate_crlf(buf, "Missing \\r\\n after the number of chars.");

            // The next num_chars number of chars can be anything
            for (size_t j = 0; j < num_chars; j++)
                buf.increment_curr_pos();

            // The next two chars have to be \r and \n
            validate_crlf(buf, "Missing \\r\\n after the number of chars.");
        }
    }
    // If we are catching a buffer out of bounds error, it means
    // that we have reached the end of the current bytes in the buffer
    // so we don't have the command
    catch (const BufferOutOfBoundsError &e)
    {
        return false;
    }

    // If we reach here, it means there is atleast one valid command
    return true;
}

std::vector<std::string> CommandParser::parse_command(Buffer &buf)
{
    // Resulting vector of command parts
    std::vector<std::string> parts;

    // Start parsing the command bytes at read_pos
    buf.reset_curr_pos();

    // First byte is '*'
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
        // This byte is '$'
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