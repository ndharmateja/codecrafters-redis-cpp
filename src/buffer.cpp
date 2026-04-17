#include <algorithm>
#include <cstring>
#include "buffer.hpp"
#include "errors.hpp"

// Constants
const size_t Buffer::MIN_CAPACITY = 1024;

Buffer::Buffer(size_t initial_size)
    : buffer(std::max(MIN_CAPACITY, initial_size)),
      read_pos{0},
      curr_pos{0},
      write_pos{0} {}

unsigned char Buffer::get_curr_char() const
{
    if (curr_pos >= write_pos)
        throw BufferOutOfBoundsError();
    return buffer[curr_pos];
}

int Buffer::find(std::string key, size_t start_index)
{
    size_t key_size = key.length();
    for (size_t i = start_index; i < write_pos - key_size; i++)
    {
        // Check if buffer[i: i + key_size) is the same as key
        bool is_match = true;
        for (size_t j = 0; j < key_size; j++)
            if (buffer[i + j] != key[j])
            {
                is_match = false;
                break;
            }

        // If it is a match, we can return i
        if (is_match)
            return i;
    }

    // If we reach here, then it means that there is no match
    return -1;
}

void Buffer::compact()
{
    // We can use std::memmove to move the bytes from [read_pos, write_pos) to [0: write_pos - read_pos)
    size_t remaining_bytes = write_pos - read_pos;

    // Move data if there are bytes to move
    if (remaining_bytes > 0)
        std::memmove(buffer.data(), buffer.data() + read_pos, remaining_bytes);

    // Update the new positions, every pos moves read_pos indices to the left
    write_pos -= read_pos;
    curr_pos -= read_pos;
    read_pos = 0;
}
