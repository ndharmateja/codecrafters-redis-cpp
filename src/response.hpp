#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <deque>

class Response
{
private:
    inline static const std::string CRLF{"\r\n"};

    // We are returning a reference to the response itself so that we can directly return
    // the output of add_item in the create_bulk_string etc
    static std::string &add_item(std::string &response, const std::string &item) { return response.append(item).append(CRLF); }
    static std::string &add_item(std::string &response, size_t item) { return response.append(std::to_string(item)).append(CRLF); }
    static std::string &add_item(std::string &response, int item) { return response.append(std::to_string(item)).append(CRLF); }

public:
    static std::string create_bulk_string(const std::string &input)
    {
        std::string response{"$"};
        add_item(response, input.length());
        return add_item(response, input);
    }

    static std::string create_simple_string(const std::string &input)
    {
        std::string response{"+"};
        return add_item(response, input);
    }

    static std::string create_simple_error(const std::string &input)
    {
        std::string response{"-"};
        return add_item(response, input);
    }

    static std::string create_null_bulk_string() { return "$-1\r\n"; };

    static std::string create_integer_string(int x)
    {
        std::string response{":"};
        return add_item(response, x);
    };

    static std::string create_array_of_bulk_strings(std::deque<std::string> values)
    {
        std::string response("*");
        add_item(response, values.size());

        for (const auto &value : values)
        {
            response.append("$");
            add_item(response, value.length());
            add_item(response, value);
        }

        return response;
    }
};

#endif