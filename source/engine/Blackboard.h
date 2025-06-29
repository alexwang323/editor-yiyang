#pragma once
#include <any>
#include <unordered_map>

class Blackboard {
public:
	template <typename T>
	void set_value(const char* key, const T& value);

	template <typename T>
	T get_value(const char* key) const;
private:
	// string pooling is enabled, so hashing the pointer directly should be fine
	std::unordered_map<const char*, std::any> data;
};

template<typename T>
inline void Blackboard::set_value(const char* key, const T& value)
{
	static_assert(std::is_copy_constructible<T>::value, "Attempting to add non-copy-constructible type into blackboard");
	data[key] = value;
}

template<typename T>
inline T Blackboard::get_value(const char* key) const 
{
	const auto result = data.find(key);

	// make sure there is a value for the key
	if (result != data.end()) {
		// try to cast the value to the expected type
		try {
			T value = std::any_cast<T>(result->second);
			return value;
		}
		catch (const std::bad_any_cast&)
		{
			// attempted to cast value to the wrong type, that means the value for this key was set to a different type
			__debugbreak();
		}
	}
	else {
		// attempted to get a value that hasn't been set yet
		__debugbreak();
	}
	return T(); // if we managed to get here, just default construct
}