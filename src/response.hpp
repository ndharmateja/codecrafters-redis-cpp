#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>

class Response
{
public:
    static std::string create_bulk_string(const std::string &input)
    {
        std::string response;
        response.append("$");
        response.append(std::to_string(input.length()));
        response.append("\r\n");
        response.append(input);
        response.append("\r\n");
        return response;
    }

    static std::string create_simple_string(const std::string &input)
    {
        std::string response;
        response.append("+");
        response.append(input);
        response.append("\r\n");
        return response;
    }

    static std::string create_simple_error(const std::string &input)
    {
        std::string response;
        response.append("-");
        response.append(input);
        response.append("\r\n");
        return response;
    }
};

#endif