#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>

class Buffer
{
private:
    // Constants
    static const size_t MIN_CAPACITY;

    // Instance variables
    std::vector<unsigned char> buffer;
    size_t read_pos;
    size_t write_pos;

public:
    // Constructor
    Buffer(size_t initial_size);

    // Instance methods
    // Getters
    size_t get_read_pos() const;
    size_t get_write_pos() const;
    size_t get_free_space() const;
    bool is_full() const;
    unsigned char *get_write_pointer();
    unsigned char *get_pointer(size_t index);
    const unsigned char *get_pointer(size_t index) const;

    // Update methods
    void increment_write_pos(size_t n);
    void set_read_pos(size_t new_read_pos);

    /**
     * Clears everything before the read_pos and resets read_pos to 0
     * Copies everything in [read_pos, write_pos) to [0: write_pos - read_pos)
     *
     * Assumes that write_pos >= read_pos
     */
    void compact();
    void increase_capacity();

    // [] operator
    // The first one is for write and the next one is for read
    unsigned char &operator[](size_t index);
    const unsigned char &operator[](size_t index) const;
};

#endif