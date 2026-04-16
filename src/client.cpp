#include <iostream>
#include <unistd.h>

#include "client.hpp"
#include "util.hpp"
#include "response.hpp"

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
        std::cout << "Command: ";
        print_vector(command);
        std::cout << std::endl;

        // Create and send back the response
        std::string response;

        // Handle PING command
        if (command.front() == "ping")
            response.append(Response::create_bulk_string("PONG"));

        // Handle ECHO command with one argument
        // Send response back as a RESP bulk string
        else if (command.front() == "echo")
            response.append(Response::create_bulk_string(command[1]));

        // Handle SET command

        // All other commands are unsupported
        else
            response.append(Response::create_simple_error("Invalid/unsupported command"));

        // Send the response
        std::cout << "Raw response: '";
        print_escaped(response);
        std::cout << "'" << std::endl;
        send(fd, response.data(), response.length(), 0);

        // After handling each command, compact the buffer with potentially
        // removing the already processed bytes
        buf.compact();
    }
}
