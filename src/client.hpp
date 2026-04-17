#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>
#include "buffer.hpp"

class Client
{
private:
    int fd;
    Buffer buf;

public:
    // Constructor and destructor
    Client(int);
    ~Client();

    // Getters
    Buffer &get_buf() { return buf; }

    // Instance methods
    void run();
};

#endif