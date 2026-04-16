#include <iostream>
#include <unistd.h>

#include "client.hpp"
#include "util.hpp"

// Constructor and destructor
Client::Client(int client_fd) : fd{client_fd}, buf{1024} {}
Client::~Client() { close(fd); }

void Client::run()
{
    // Handle commands indefinitely
    while (true)
    {
        // If receive_one_command returns false, it means that either there is an error
        // or the client closed the connection
        if (!receive_one_command(fd, buf))
            break;

        // Parse the command
        std::vector<std::string> command = parse_command(buf);

        // Print the command received
        for (const std::string &part : command)
            std::cout << part << " ";
        std::cout << std::endl;

        // Create and send back the response
        const char *data = "+PONG\r\n";
        send(fd, data, strlen(data), 0);

        // After handling each command, compact the buffer with potentially
        // removing the already processed bytes
        buf.compact();
    }
}
