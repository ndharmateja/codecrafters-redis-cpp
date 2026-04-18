#include "key_value_store.hpp"
#include "errors.hpp"

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
    // If key doesn't exist, return nullopt
    auto it = db_map.find(key);
    if (it == db_map.end())
        return std::nullopt;

    // If expired, we delete the value from the db and return nullopt
    const RedisValueObject &value = it->second;
    if (value.has_expiry() && value.has_expired())
    {
        db_map.erase(it);
        return std::nullopt;
    }

    // Type check for string and return
    auto *str_ptr = std::get_if<std::string>(&value.get_data());
    if (str_ptr)
        return *str_ptr;

    // If str_ptr is nullptr it means that it is not of a string value
    throw WrongTypeError("Not a string value.");

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
    std::lock_guard<std::mutex> lock(db_mutex);

    // Create the new value and assign it to the key
    // This will *copy* the value
    // This will overwrite the value (even if it were a list)
    // TODO: move semantics using std::move to avoid copying
    db_map.insert_or_assign(key, RedisValueObject(value, expiry));

    // The mutex is automatically unlocked here (RAII)
    // In all cases of error, key found and not found
}

int KeyValueStore::push_back_list_values(const std::string &key,
                                         const std::deque<std::string> &values,
                                         std::optional<long long> expiry_in_ms)
{
    // Store the expiry time as a timestamp
    std::optional<timestamp> expiry;
    if (expiry_in_ms.has_value())
        // 2. Add duration to current time
        expiry = std::chrono::steady_clock::now() + std::chrono::milliseconds(*expiry_in_ms);

    // Lock mutex
    std::lock_guard<std::mutex> lock(db_mutex);

    // Try and find the key
    auto it = db_map.find(key);

    // If the key doesn't exist
    // Create a new deque with solely this value and put it in the list
    if ((it == db_map.end()))
    {
        // Create the new deque
        std::deque<std::string> new_list{values};

        // Emplace the new value and return 1 which is the length of the new list
        db_map.emplace(key, RedisValueObject(new_list, expiry));
        return new_list.size();
    }

    // If the key exists and it has expired, we do the same thing
    RedisValueObject &redis_value = it->second;
    if (redis_value.has_expiry() && redis_value.has_expired())
    {
        // Create the new deque
        std::deque<std::string> new_list{values};
        it->second = redis_value;

        // we return 1 as it is the length of the new list
        return 1;
    }

    // At this point the key exists and has not expired or doesn't have expiry
    // so we check the type
    auto *list_ptr = std::get_if<std::deque<std::string>>(&redis_value.get_data_mutable());
    if (list_ptr)
    {
        for (const auto &value : values)
            list_ptr->push_back(value);

        // Return the new size of the list
        return list_ptr->size();
    }

    // If we reach here it means that the value is not of list type
    throw WrongTypeError("Not a string value.");

    // The mutex is automatically unlocked here (RAII)
    // In all cases of error, key found and not found
}