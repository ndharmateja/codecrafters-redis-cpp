#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <unordered_map>
#include <mutex>
#include <string>
#include <optional>

typedef std::chrono::steady_clock::time_point timestamp;

class RedisValueObject
{
private:
    std::string value;
    std::optional<timestamp> expiry_time;

public:
    RedisValueObject(std::string value, std::optional<timestamp> expiry)
        : value{value}, expiry_time{expiry} {}
    std::string get_value() const { return value; }
    std::optional<timestamp> get_expiry_time() const { return expiry_time; };
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
    std::unordered_map<std::string, RedisValueObject> map;

    // We make the mutex mutable so that we can use const getters
    // even though we are technically changing the mutex during the get
    // method
    mutable std::mutex db_mutex;

    // Private constructor for singleton pattern
    KeyValueStore() {}

    // Delete the assignment and the copy constructors
    KeyValueStore(const KeyValueStore &) = delete;
    KeyValueStore &operator=(const KeyValueStore &) = delete;

public:
    // static method for singleton pattern
    static KeyValueStore &get_instance();

    // Instance methods
    // Using the const & is okay for the keys and values as
    // when we are inserting them into the map, deep copies of the strings would be created
    std::optional<std::string> get_value(const std::string &key) const;
    void set_value(const std::string &key, const std::string &value);
};

#endif