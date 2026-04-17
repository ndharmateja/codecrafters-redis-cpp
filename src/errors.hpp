#include <stdexcept>
#include <string>

class RedisError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class ClientConnectionError : public RedisError
{
public:
    ClientConnectionError() : RedisError("Client connection error.") {}
};

class CommandParseError : public RedisError
{
public:
    explicit CommandParseError(const std::string &msg)
        : RedisError("Protocol Error: " + msg) {}
};