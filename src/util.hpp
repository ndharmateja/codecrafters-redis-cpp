#include <vector>
#include <sys/socket.h>

#include "buffer.hpp"

/**
 * For now returns if there are atleast 14 chars in the buffer
 */
bool has_atleast_one_command(const Buffer &buf) { return buf.get_write_pos() - buf.get_read_pos() >= 14; }

bool receive_one_command(int client_fd, Buffer &buf)
{
    // Receive bytes until there is atleast one command in the buffer
    while (!has_atleast_one_command(buf))
    {
        int num_bytes = recv(client_fd, buf.get_write_pointer(), buf.get_free_space(), 0);

        // If the client closes, then number of bytes received would be 0
        if (num_bytes == 0)
            return false;

        // If the num bytes is -1, there is an error
        // TODO: error handling
        // For now we are returning an empty command
        if (num_bytes < 0)
            return false;

        // If the num_bytes > 0, we received some bytes. so we update curr_pos and
        // check if we have one command
        buf.increment_write_pos(num_bytes);
    }

    // At this point we have atleast one command in the buffer starting at read_pos
    return true;
}

/**
 * read_pos is the start of the bytes of the redis command
 *
 * If input is bytes: *3\r\n$3\r\nSET\r\n$3\r\nfoo\r\n$3\r\nbar\r\n
 * output will be ["SET", "foo", "bar"]
 *
 */
std::vector<std::string> parse_command(Buffer &buf)
{
    // Resulting vector of command parts
    std::vector<std::string> parts;

    // Start parsing the command bytes at read_pos
    size_t curr_pos = buf.get_read_pos();

    // If the first byte is not a *, we can raise an error
    if (buf[curr_pos++] != '*')
        throw std::domain_error("Invalid command bytes");

    // Parse the number words in the command using Horner's method
    unsigned char c;
    size_t num_words = 0;
    while (std::isdigit((c = buf[curr_pos])))
    {
        num_words = num_words * 10 + (c - '0');
        curr_pos++;
    }

    // curr_pos will be at \r now, so we can increment it by 2 to get to the $
    curr_pos += 2;

    // Loop over the number of words and parse each word
    for (size_t i = 0; i < num_words; i++)
    {
        // If curr_pos is not a $ we can raise an error
        if (buf[curr_pos++] != '$')
            throw std::domain_error("Invalid command bytes");

        // Parse the number of chars in the current part in the same way as above
        size_t num_chars = 0;
        while (std::isdigit((c = buf[curr_pos])))
        {
            num_chars = num_chars * 10 + (c - '0');
            curr_pos++;
        }

        // curr_pos will be at \r now, so we can increment it by 2 to get to the $
        curr_pos += 2;

        // Parse num_chars number of characters as the part in the range [curr_pos, curr_pos + num_chars)
        std::string part(reinterpret_cast<char *>(buf.get_pointer(curr_pos)), num_chars);
        parts.push_back(part);

        // curr_pos has to be incremented by 'num_chars + 2' to account for the \r\n at the end of the part
        curr_pos += (num_chars + 2);
    }

    // Update the read_pos cursor to curr_pos and return the command parts
    buf.set_read_pos(curr_pos);
    return parts;
}