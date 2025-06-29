#pragma once
#include "ginkgopch.h"
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <optional>

class JSONmanager {
public:
	static JSONmanager& GetInstance();
	// This findValue function is for not modifying the value
	std::optional<std::reference_wrapper<const rapidjson::Value>> findValue(const char* key);
	bool Init(const std::string& infilename);
	bool ResetReadStream(const std::string& new_filename);
	~JSONmanager();

	bool OpenWriteStream(const std::string& outFilename);
	bool EndWriteStream();

	bool StartNewObject(const std::string& key); // not all objects need to have a key
	bool EndObject();

	bool StartNewArray(const std::string& key);
	bool EndArray();

	template <typename TValue>
	bool AddKeyValuePair(const std::string& key, const TValue& value);
	template <>
	bool AddKeyValuePair(const std::string& key, const bool& value);

private:
	JSONmanager();
	std::ifstream ReadFilestream;
	std::ofstream WriteFilestream;
	rapidjson::Document deserialize_data;
	rapidjson::StringBuffer m_StringBuffer;
	std::unique_ptr<rapidjson::PrettyWriter<rapidjson::StringBuffer>> m_pWriter;
	int m_NumObjectsStarted, m_NumArraysStarted;
};

#include "jsonmanager.inl"