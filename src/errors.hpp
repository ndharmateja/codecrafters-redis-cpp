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

class BufferOutOfBoundsError : public RedisError
{
public:
    BufferOutOfBoundsError() : RedisError("Client connection error.") {}
};

class InvalidCommandStructureError : public RedisError
{
public:
    explicit InvalidCommandStructureError(const std::string &msg)
        : RedisError(msg) {}
};