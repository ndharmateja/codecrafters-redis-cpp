#include <iostream>
#include <unistd.h>

#include "client.hpp"
#include "util.hpp"
#include "response.hpp"
#include "key_value_store.hpp"
#include "command_parser.hpp"
#include "errors.hpp"
#include "commands/command_factory.hpp"

// Constructor and destructor
Client::Client(int client_fd) : fd{client_fd}, buf{1024} {}
Client::~Client() { close(fd); }

void Client::run()
{
    // Handle commands indefinitely
    while (true)
    {
        try
        {
            // If a full command isn't present in the buffer, we keep receiving more
            // bytes until there is one command in the buffer.
            // If there is an error while receiving bytes from the client, the receive_bytes
            // will throw an error and if the client closes the connection, we exit run.
            // If the command structure is invalid, has_one_command throws an error
            while (!CommandParser::has_one_command(buf))
                // If receive_bytes returns false, it means that client closed the connection
                // in which case we can exit the run method
                if (!receive_bytes())
                    return;

            // At this point there is atleast one valid full command in the buffer
            // Parse the command
            std::vector<std::string> command = CommandParser::parse_command(buf);

            // Print the command received
            std::cout << "Command: ";
            print_vector(command);
            std::cout << std::endl;

            // Create and send back the response
            std::string response;

            // Create command and response
            std::unique_ptr<Command> cmd = CommandFactory::create_command(command);
            response = cmd->execute();

            // Send the response
            std::cout << "Raw response: '";
            print_escaped(response);
            std::cout << "'" << std::endl;
            send(fd, response.data(), response.length(), 0);

            // After handling each command, compact the buffer with potentially
            // removing the already processed bytes
            buf.compact();
        }
        catch (const ClientConnectionError &e)
        {
            std::cout << "Client disconnected" << std::endl;
            return;
        }
        catch (const RedisError &e)
        {
            std::string error_resp = Response::create_simple_error(e.what());
            std::cout << "Error parsing command: " << e.what() << std::endl;
            send(fd, error_resp.data(), error_resp.length(), 0);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Unexpected system error: " << e.what() << std::endl;
            return;
        }
    }
}

bool Client::receive_bytes()
{
    // If there aren't atleast 128 bytes we first try and compact it, otherwise we resize it
    if (!buf.has_free_bytes(128))
    {
        buf.compact();

        // we have this if statement nested so that in general cases we don't have to do
        // 2 separate if checks
        if (!buf.has_free_bytes(128))
            buf.increase_capacity();
    }

    // Receive bytes
    int num_bytes = recv(fd, buf.get_write_pointer(), buf.get_free_bytes(), 0);

    // If the client closes, then number of bytes received would be 0
    if (num_bytes == 0)
        return false;

    // If the num bytes is -1, there is an error
    // For now we are returning an empty command
    if (num_bytes < 0)
        throw ClientConnectionError();

    // If the num_bytes > 0, we received some bytes. so we update write_pos
    buf.increment_write_pos(num_bytes);
    return true;
}