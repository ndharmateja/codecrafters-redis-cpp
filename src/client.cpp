#include <iostream>
#include <unistd.h>

#include "client.hpp"
#include "util.hpp"

// Constructor and destructor
Client::Client(int client_fd) : fd{client_fd}, buffer(1024), read_pos{0}, buffer_end_pos{0} {}
Client::~Client() { close(fd); }

void Client::run()
{
    // Handle commands indefinitely
    while (true)
    {
        // If receive_one_command returns false, it means that either there is an error
        // or the client closed the connection
        if (!receive_one_command(fd, buffer, read_pos, buffer_end_pos))
            break;

        // Parse the command
        std::vector<std::string> command = parse_command(buffer, read_pos);

        // Print the command received
        for (const std::string &part : command)
            std::cout << part << " ";
        std::cout << std::endl;

        // Create and send back the response
        const char *data = "+PONG\r\n";
        send(fd, data, strlen(data), 0);

        // After handling each command,
        // if more than half the buffer is full, we clear it
        if (read_pos >= buffer.size() / 2)
            clear_buffer(buffer, read_pos, buffer_end_pos);
    }
}
