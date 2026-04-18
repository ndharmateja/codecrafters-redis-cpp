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
    db_map.insert_or_assign(key, RedisValueObject(value, expiry));

    // The mutex is automatically unlocked here (RAII)
    // In all cases of error, key found and not found
}

void KeyValueStore::push_value(std::deque<std::string> &values, const std::string &value, Side side)
{
    if (side == Side::Front)
        values.push_front(value);
    else
        values.push_back(value);
}

int KeyValueStore::push_list_values(const std::string &key,
                                    const std::deque<std::string> &values, Side side)
{
    // Lock mutex
    std::lock_guard<std::mutex> lock(db_mutex);

    // Try and find the key
    auto it = db_map.find(key);

    // If the key doesn't exist
    // Create a new deque with solely this value and put it in the list
    if ((it == db_map.end()))
    {
        // Create the new deque
        std::deque<std::string> new_list;
        for (const auto &value : values)
            push_value(new_list, value, side);

        // Emplace the new value and return 1 which is the length of the new list
        db_map.emplace(key, RedisValueObject(new_list));
        return new_list.size();
    }

    // The key exists, so we check the type
    RedisValueObject &redis_value = it->second;
    auto *list_ptr = std::get_if<std::deque<std::string>>(&redis_value.get_data_mutable());
    if (list_ptr)
    {
        for (const auto &value : values)
            push_value(*list_ptr, value, side);

        // Return the new size of the list
        return list_ptr->size();
    }

    // If we reach here it means that the value is not of list type
    throw WrongTypeError("Not a list value.");

    // The mutex is automatically unlocked here (RAII)
    // In all cases of error, key found and not found
}

std::deque<std::string> KeyValueStore::get_list_values(const std::string &key, int start, int stop)
{
    // Lock mutex
    std::lock_guard<std::mutex> lock(db_mutex);

    // Find and return the corresponding value if key exists
    // If key doesn't exist, return nullopt
    auto it = db_map.find(key);
    if (it == db_map.end())
        return std::deque<std::string>(0);

    // Type check for list and return
    const RedisValueObject &value = it->second;
    auto *list_ptr = std::get_if<std::deque<std::string>>(&value.get_data());
    if (list_ptr)
    {
        int num_elements = list_ptr->size();
        // If a negative index is out of range, we treat it as 0
        if (start < -num_elements)
            start = 0;
        if (stop < -num_elements)
            stop = 0;

        // Convert start and stop indices to positive values
        if (start < 0)
            start += num_elements;
        if (stop < 0)
            stop += num_elements;

        // Slice the deque from [start:stop]
        // If start >= num_elements, empty list needs to be returned
        if (start >= num_elements)
            return std::deque<std::string>(0);

        // If stop >= num_elements, stop must be considered as the last element
        if (stop >= num_elements)
            stop = num_elements - 1;

        // If start > stop, empty list needs to be returned
        if (start > stop)
            return std::deque<std::string>(0);

        // Slice the list
        // We need (stop + 1) as stop is inclusive
        return std::deque<std::string>(
            list_ptr->begin() + start,
            list_ptr->begin() + (stop + 1));
    }

    // If list_ptr is nullptr it means that it is not of a list value
    throw WrongTypeError("Not a list value.");

    // The mutex is automatically unlocked here (RAII)
    // In all cases of error, key found and not found
}

int KeyValueStore::get_list_length(const std::string &key)
{
    // Lock mutex
    std::lock_guard<std::mutex> lock(db_mutex);

    // Find and return the corresponding value if key exists
    // If key doesn't exist, return 0
    auto it = db_map.find(key);
    if (it == db_map.end())
        return 0;

    // If key exists, type check for list and return
    const RedisValueObject &value = it->second;
    auto *list_ptr = std::get_if<std::deque<std::string>>(&value.get_data());
    if (list_ptr)
        return list_ptr->size();

    // If list_ptr is nullptr it means that it is not of a list value
    throw WrongTypeError("Not a list value.");

    // The mutex is automatically unlocked here (RAII)
    // In all cases of error, key found and not found
}

std::deque<std::string> KeyValueStore::pop_front_list_values(const std::string &key, int num_values_to_pop)
{
    // Lock mutex
    std::lock_guard<std::mutex> lock(db_mutex);

    // Find the corresponding value if key exists
    // If key doesn't exist, return empty deque
    auto it = db_map.find(key);
    if (it == db_map.end())
        return std::deque<std::string>(0);

    // Type check for list and return
    RedisValueObject &value = it->second;
    auto *list_ptr = std::get_if<std::deque<std::string>>(&value.get_data_mutable());
    if (list_ptr)
    {
        // Result for storing the popped elements
        std::deque<std::string> result;
        int num_elements = list_ptr->size();

        // If the num_values to delete >= the number of elements in the list
        // we return the whole list and we can delete the key
        if (num_values_to_pop >= num_elements)
        {
            result = *list_ptr;
            db_map.erase(it);
        }

        // If the num_values to delete < number of elements in the list
        // we can slice the list and remove them from the list
        else
        {
            // Create teh sliced deque
            result = std::deque<std::string>(
                list_ptr->begin(),
                list_ptr->begin() + num_values_to_pop);

            // Remove those elements from the original deque
            list_ptr->erase(list_ptr->begin(), list_ptr->begin() + num_values_to_pop);
        }

        // Return the list of removed elements
        return result;
    }

    // If list_ptr is nullptr it means that it is not of a list value
    throw WrongTypeError("Not a list value.");

    // The mutex is automatically unlocked here (RAII)
    // In all cases of error, key found and not found
}