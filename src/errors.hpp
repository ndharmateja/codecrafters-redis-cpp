#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <stdexcept>
#include <string>

class ApplicationError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class RedisError : public ApplicationError
{
public:
    RedisError(const std::string &msg) : ApplicationError(msg) {}
};

class CommandParseError : public RedisError
{
public:
    explicit CommandParseError(const std::string &msg)
        : RedisError(msg) {}
};

class InvalidCommandStructureError : public RedisError
{
public:
    explicit InvalidCommandStructureError(const std::string &msg)
        : RedisError(msg) {}
};

class WrongTypeError : public RedisError
{
public:
    explicit WrongTypeError(const std::string &msg)
        : RedisError(msg) {}
};

// Non redis errors
class ClientConnectionError : public RedisError
{
public:
    ClientConnectionError() : RedisError("Client connection error.") {}
};

class BufferOutOfBoundsError : public RedisError
{
public:
    BufferOutOfBoundsError() : RedisError("Buffer out of bounds error.") {}
};

#endif