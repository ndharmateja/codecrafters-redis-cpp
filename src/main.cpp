#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>

/**
 * Clears everything before the read_pos and resets read_pos to 0
 * Copies everything in [read_pos, buffer_end_pos) to [0: buffer_end_pos - read_pos)
 *
 * Assumes that buffer_end_pos >= read_pos
 */
void clear_buffer(std::vector<char> &buffer, size_t &read_pos, size_t &buffer_end_pos)
{
    // We can use std::memmove to move the bytes from [read_pos, buffer_end_pos) to [0: buffer_end_pos - read_pos)
    size_t remaining_bytes = buffer_end_pos - read_pos;

    // Move data if there are bytes to move
    if (remaining_bytes > 0)
        std::memmove(buffer.data(), buffer.data() + read_pos, remaining_bytes);

    // Update the new positions
    buffer_end_pos = remaining_bytes;
    read_pos = 0;
}

/**
 * For now returns if there are atleast 14 chars in the buffer
 */
bool has_atleast_one_command(std::vector<char> &buffer, size_t &read_pos, const size_t &buffer_end_pos)
{
    return buffer_end_pos - read_pos >= 14;
}

bool receive_one_command(int client_fd, std::vector<char> &buffer, size_t &read_pos, size_t &buffer_end_pos)
{
    // Receive bytes until there is atleast one command in the buffer
    while (!has_atleast_one_command(buffer, read_pos, buffer_end_pos))
    {
        int num_bytes = recv(client_fd, buffer.data() + buffer_end_pos, buffer.size() - buffer_end_pos, 0);

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
        buffer_end_pos += num_bytes;
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
std::vector<std::string> parse_command(const std::vector<char> &buffer, size_t &read_pos)
{
    // Resulting vector of command parts
    std::vector<std::string> parts;

    // Start parsing the command bytes at read_pos
    size_t curr_pos = read_pos;

    // If the first byte is not a *, we can raise an error
    if (buffer[curr_pos++] != '*')
        throw std::domain_error("Invalid command bytes");

    // Parse the number words in the command using Horner's method
    unsigned char c;
    size_t num_words = 0;
    while (std::isdigit((c = buffer[curr_pos])))
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
        if (buffer[curr_pos++] != '$')
            throw std::domain_error("Invalid command bytes");

        // Parse the number of chars in the current part in the same way as above
        size_t num_chars = 0;
        while (std::isdigit((c = buffer[curr_pos])))
        {
            num_chars = num_chars * 10 + (c - '0');
            curr_pos++;
        }

        // curr_pos will be at \r now, so we can increment it by 2 to get to the $
        curr_pos += 2;

        // Parse num_chars number of characters as the part in the range [curr_pos, curr_pos + num_chars)
        std::string part(buffer.data() + curr_pos, num_chars);
        parts.push_back(part);

        // curr_pos has to be incremented by 'num_chars + 2' to account for the \r\n at the end of the part
        curr_pos += (num_chars + 2);
    }

    // Update the read_pos cursor to curr_pos and return the command parts
    read_pos = curr_pos;
    return parts;
}

void handle_client(int client_fd)
{
    // Create the buffer
    std::vector<char> buffer(1024);
    size_t read_pos = 0;
    size_t buffer_end_pos = 0;

    // Handle commands indefinitely
    while (true)
    {
        // If receive_one_command returns false, it means that either there is an error
        // or the client closed the connection
        if (!receive_one_command(client_fd, buffer, read_pos, buffer_end_pos))
            break;

        // Parse the command
        std::vector<std::string> command = parse_command(buffer, read_pos);

        // Print the command received
        for (const std::string &part : command)
            std::cout << part << " ";
        std::cout << std::endl;

        // Create and send back the response
        const char *data = "+PONG\r\n";
        send(client_fd, data, strlen(data), 0);
    }

    // Close the client connection
    close(client_fd);
}

int main(int argc, char **argv)
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }

    // Since the tester restarts your program quite often, setting SO_REUSEADDR
    // ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        std::cerr << "setsockopt failed\n";
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6379);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
    {
        std::cerr << "Failed to bind to port 6379\n";
        return 1;
    }

    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0)
    {
        std::cerr << "listen failed\n";
        return 1;
    }

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    std::cout << "Waiting for a client to connect...\n";

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cout << "Logs from your program will appear here!\n";

    // Run the indefinite loop
    while (true)
    {
        // Accept a client connection
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
        std::cout << "Client connected\n";

        // Handle the client
        handle_client(client_fd);
    }

    // Close the client socket
    // Close the server listening socket
    close(server_fd);

    return 0;
}
