#include "jsonmanager.h"


/*
* Allowing the compiler to pick the type for us
*/
template<typename TValue>
inline bool JSONmanager::AddKeyValuePair(const std::string& key, const TValue& value)
{
	// Add key first
	if(key.size() != 0)
		m_pWriter->Key(key.c_str());
	// compiler figuring out the type
	if constexpr (std::is_same<TValue, std::string>::value) {
		// if the value is a string
		return m_pWriter->String(value.c_str());
	}
	else if constexpr (std::is_integral_v<TValue>) {
		return m_pWriter->Int(value);
	}
	else if constexpr (std::is_unsigned_v<TValue>) {
		return m_pWriter->Uint(value);
	}
	else if constexpr (std::is_floating_point_v<TValue>) {
		return m_pWriter->Double(value);
	}
	else {
		std::cout << "Type is not supported! " << std::endl;
		return false;
	}
}

template <>
inline bool JSONmanager::AddKeyValuePair(const std::string& key, const bool& value)
{
	m_pWriter->Key(key.c_str());
	return m_pWriter->Bool(value);
}

