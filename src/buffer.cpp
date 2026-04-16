#include "buffer.hpp"

Buffer::Buffer(int initial_size) : buffer(initial_size), read_pos{0}, write_pos{0} {}

// Instance methods
size_t Buffer::get_read_pos() const { return read_pos; }
size_t Buffer::get_write_pos() const { return write_pos; }
size_t Buffer::get_free_space() const { return buffer.size() - write_pos; }
bool Buffer::is_full() const { return write_pos == buffer.size(); }

unsigned char *Buffer::get_write_pointer() { return buffer.data() + write_pos; }
unsigned char *Buffer::get_pointer(size_t index) { return buffer.data() + index; }
const unsigned char *Buffer::get_pointer(size_t index) const { return buffer.data() + index; }

void Buffer::increment_write_pos(size_t n) { write_pos += n; }
void Buffer::set_read_pos(size_t new_read_pos) { read_pos = new_read_pos; }

void Buffer::compact()
{
    // Only compact the buffer if more than half of the buffer is occupied
    if (buffer.size() - write_pos >= buffer.size() / 2)
        return;

    // We can use std::memmove to move the bytes from [read_pos, write_pos) to [0: write_pos - read_pos)
    size_t remaining_bytes = write_pos - read_pos;

    // Move data if there are bytes to move
    if (remaining_bytes > 0)
        std::memmove(buffer.data(), buffer.data() + read_pos, remaining_bytes);

    // Update the new positions
    write_pos = remaining_bytes;
    read_pos = 0;
}

void Buffer::increase_capacity() { buffer.resize(buffer.size() * 2); }

unsigned char &Buffer::operator[](size_t index) { return buffer[index]; }
const unsigned char &Buffer::operator[](size_t index) const { return buffer[index]; }