#include "key_value_store.hpp"

KeyValueStore &KeyValueStore::get_instance()
{
    // Meyer's singleton pattern
    // Threadsafe since C++ 11, so we don't need to have
    // a mutex to access the get_instance() method code
    static KeyValueStore instance;
    return instance;
}

std::optional<std::string> KeyValueStore::get_value(const std::string &key)
{
    // Lock mutex
    std::lock_guard<std::mutex> lock(db_mutex);

    // Find and return the corresponding value if key exists
    auto it = map.find(key);
    if (it == map.end())
        return std::nullopt;

    // If no expiry or hasn't expired yet, we just return the value
    const RedisValueObject &value = it->second;
    if (!value.get_expiry_time().has_value() || *(value.get_expiry_time()) > std::chrono::steady_clock::now())
        return value.get_value();

    // If expired, we delete the value from the db and return nullopt
    map.erase(it);
    return std::nullopt;

    // The mutex is automatically unlocked here (RAII)
    // In all cases of error, key found and not found
}

void KeyValueStore::set_value(const std::string &key, const std::string &value)
{
    // Lock mutex
    std::lock_guard<std::mutex> lock(db_mutex);

    // Set the key:value
    map[key] = value;

    // The mutex is automatically unlocked here (RAII)
    // In all cases of error, key found and not found
}