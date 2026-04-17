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
    std::lock_guard<std::mutex> lock(str_map_mutex);

    // Find and return the corresponding value if key exists
    auto it = str_value_map.find(key);
    if (it == str_value_map.end())
        return std::nullopt;

    // If no expiry or hasn't expired yet, we just return the value
    const RedisValueObject<std::string> &value = it->second;
    if (!value.get_expiry_time().has_value() || *(value.get_expiry_time()) > std::chrono::steady_clock::now())
        return value.get_value();

    // If expired, we delete the value from the db and return nullopt
    str_value_map.erase(it);
    return std::nullopt;

    // The mutex is automatically unlocked here (RAII)
    // In all cases of error, key found and not found
}

void KeyValueStore::set_value(const std::string &key, const std::string &value, std::optional<long long> expiry_in_ms)
{
    // Store the expiry time as a timestamp
    std::optional<timestamp> expiry;
    if (expiry_in_ms.has_value())
        // 2. Add duration to current time
        expiry = std::chrono::steady_clock::now() + std::chrono::milliseconds(*expiry_in_ms);

    // Lock mutex
    std::lock_guard<std::mutex> lock(str_map_mutex);

    // Set the key:value
    str_value_map.insert_or_assign(key, RedisValueObject(value, expiry));

    // The mutex is automatically unlocked here (RAII)
    // In all cases of error, key found and not found
}