#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <vector>

class Client
{
private:
    int fd;
    std::vector<char> buffer;
    size_t read_pos, buffer_end_pos;

public:
    // Constructor and destructor
    Client(int);
    ~Client();

    // Instance methods
    void run();
};

#endif