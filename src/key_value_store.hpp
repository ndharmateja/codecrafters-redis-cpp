#ifndef KEY_VALUE_STORE_HPP
#define KEY_VALUE_STORE_HPP

#include <unordered_map>
#include <mutex>
#include <string>
#include <variant>
#include <deque>
#include <optional>

typedef std::chrono::steady_clock::time_point timestamp;
typedef std::variant<std::string, std::deque<std::string>> RedisData;

enum class Side
{
    Front,
    Back
};

class RedisValueObject
{
private:
    RedisData data;
    std::optional<timestamp> expiry_time;

public:
    RedisValueObject(RedisData value, std::optional<timestamp> expiry)
        : data{value}, expiry_time{expiry} {}
    RedisValueObject(RedisData value) : data{value} {}

    RedisData &get_data_mutable() { return data; }
    const RedisData &get_data() const { return data; }
    std::optional<timestamp> get_expiry_time() const { return expiry_time; };

    // type checking
    bool is_string() const { return std::holds_alternative<std::string>(data); }
    bool is_deque() const { return std::holds_alternative<std::deque<std::string>>(data); }

    // Expiry
    bool has_expiry() const { return expiry_time.has_value(); }

    /**
     * Assumes that there is an expiry
     * Use it with has_expiry()
     */
    bool has_expired() const { return *(expiry_time) < std::chrono::steady_clock::now(); }
};

/**
 * We can use the singleton pattern for the KeyValueStore class as there is only going
 * to be one instance of this class throughout the lifetime of the program
 * atleast for now.
 */
class KeyValueStore
{
    // For now we are just storing a string to string mapping
    // in the future we expand it for lists etc
    // Which is why we are encapsulating in a DB class
private:
    std::unordered_map<std::string, RedisValueObject> db_map;

    // We make the mutex mutable so that we can use const getters
    // even though we are technically changing the mutex during the get
    // method
    mutable std::mutex db_mutex;

    // Private constructor for singleton pattern
    KeyValueStore() {}

    // Delete the assignment and the copy constructors
    KeyValueStore(const KeyValueStore &) = delete;
    KeyValueStore &operator=(const KeyValueStore &) = delete;

    // Helper methods
    int push_list_values(const std::string &key,
                         const std::deque<std::string> &values, Side side);
    static void push_value(std::deque<std::string> &values, const std::string &value, Side side);

public:
    // static method for singleton pattern
    static KeyValueStore &get_instance();

    // Instance methods
    // Using the const & is okay for the keys and values as
    // when we are inserting them into the map, deep copies of the strings would be created
    std::optional<std::string> get_value(const std::string &key);
    void set_value(const std::string &key,
                   const std::string &value,
                   std::optional<long long> expiry_in_ms);

    // List methods
    int push_back_list_values(const std::string &key,
                              const std::deque<std::string> &values) { return push_list_values(key, values, Side::Back); }
    int push_front_list_values(const std::string &key,
                               const std::deque<std::string> &values) { return push_list_values(key, values, Side::Front); }
    std::deque<std::string> get_list_values(const std::string &key, int start = 0, int stop = -1);
    int get_list_length(const std::string &key);
};

#endif