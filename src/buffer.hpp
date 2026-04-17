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
    size_t curr_pos;
    size_t write_pos;

public:
    // Constructor
    Buffer(size_t initial_size);

    // Instance methods
    // Getters
    size_t get_read_pos() const { return read_pos; }
    size_t get_curr_pos() const { return curr_pos; }
    size_t get_write_pos() const { return write_pos; }
    unsigned char get_curr_char() const;

    bool has_bytes_from_curr(size_t num_bytes) { return write_pos - curr_pos >= num_bytes; }
    bool is_curr_in_bounds() { return curr_pos < write_pos; }

    // Instance methods about space
    size_t get_free_bytes() const { return buffer.size() - write_pos; }
    size_t has_free_bytes(size_t num_bytes) const { return buffer.size() - write_pos >= num_bytes; }
    bool is_full() const { return write_pos == buffer.size(); }

    // Instance methods about pointers
    unsigned char *get_write_pointer() { return buffer.data() + write_pos; }
    unsigned char *get_pointer(size_t index) { return buffer.data() + index; }
    const unsigned char *get_pointer(size_t index) const { return buffer.data() + index; }

    // Update methods
    void increment_write_pos(size_t n) { write_pos += n; }

    // TODO: add error handling
    void increment_curr_pos(size_t n) { curr_pos += n; }
    void increment_curr_pos() { curr_pos++; }
    void set_read_pos(size_t new_read_pos) { read_pos = new_read_pos; }
    /**
     * Resets the curr_pos to the read_pos
     */
    void reset_curr_pos() { curr_pos = read_pos; }
    void set_curr_pos(size_t new_curr_pos) { curr_pos = new_curr_pos; }

    // Find methods
    /**
     * Finds and returns the index of the first occurrence of key
     * in the buffer starting from start_index
     *
     * Runs in O(nk)
     * where n: length of the buffer and k: length of the key
     */
    int find(std::string key, size_t start_index);

    /**
     * Finds and returns the index of the first occurrence of key
     * in the buffer starting from curr_pos
     *
     * Runs in O(nk)
     * where n: length of the buffer and k: length of the key
     */
    int find(std::string key) { return find(key, curr_pos); }

    /**
     * Clears everything before the read_pos and resets read_pos to 0
     * Copies everything in [read_pos, write_pos) to [0: write_pos - read_pos)
     *
     * Assumes that write_pos >= read_pos
     */
    void compact();
    void increase_capacity() { buffer.resize(buffer.size() * 2); }

    // [] operator
    // The first one is for write and the next one is for read
    unsigned char &operator[](size_t index) { return buffer[index]; }
    const unsigned char &operator[](size_t index) const { return buffer[index]; }
};

#endif