#include <vector>
#include <sys/socket.h>

#include "buffer.hpp"
#include <algorithm>
#include <cctype>

/**
 * Function to print a string with escaped characters
 */
void print_escaped(const std::string &str)
{
    for (char c : str)
    {
        switch (c)
        {
        case '\r':
            std::cout << "\\r";
            break;
        case '\n':
            std::cout << "\\n";
            break;
        default:
            std::cout << c;
            break;
        }
    }
}

void print_vector(const std::vector<std::string> &vec)
{
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        std::cout << "\"" << vec[i] << "\"";
        if (i < vec.size() - 1)
        {
            std::cout << ", ";
        }
    }
    std::cout << "]";
}