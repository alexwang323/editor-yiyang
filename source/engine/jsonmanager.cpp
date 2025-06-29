#include "ginkgopch.h"
#include "jsonmanager.h"
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>
#include <format>

constexpr int MAX_DECIMAL_PLACES = 3;

JSONmanager& JSON_MANAGAER = JSONmanager::GetInstance();

JSONmanager& JSONmanager::GetInstance() {
	static JSONmanager instance;
	return instance;
}

JSONmanager::JSONmanager()
	:m_StringBuffer{},
	m_pWriter{ std::make_unique<rapidjson::PrettyWriter<rapidjson::StringBuffer>>(m_StringBuffer)},
	m_NumObjectsStarted{0},
	m_NumArraysStarted{0}
{}

bool JSONmanager::Init(const std::string& infilename) {
	// Init is only reading in data for deserialization 
	ReadFilestream.open(infilename, std::ios::in);
	if (!ReadFilestream.is_open())
		throw std::runtime_error(std::format("JSON manager failed to open Readfile %s", infilename));

	// Check if the file is empty
	ReadFilestream.seekg(0, std::ios::end);
	if (ReadFilestream.tellg() == 0) {
		std::cerr << "Error: File is empty!" << infilename << std::endl;
	}
	ReadFilestream.seekg(0, std::ios::beg);  // Reset to the start of the file

	rapidjson::IStreamWrapper isw_d(ReadFilestream);

	if (deserialize_data.ParseStream(isw_d).HasParseError()) {
		std::cerr << "Parse error at offset "
			<< deserialize_data.GetErrorOffset() << ": "
			<< rapidjson::GetParseError_En(deserialize_data.GetParseError())
			<< std::endl;
		throw std::runtime_error(std::format("File has parse error {}", infilename));
	}

	if (!deserialize_data.IsObject()) {
		std::cerr << "[Error] JSON root is not an object. filename provided: " << infilename << std::endl;
	}


	ReadFilestream.close();
	return true;
}

std::optional<std::reference_wrapper<const rapidjson::Value>> JSONmanager::findValue(const char* key)
{
	rapidjson::Value::ConstMemberIterator itr;
	itr = deserialize_data.FindMember(key);
	if (itr != deserialize_data.MemberEnd()) {
		// key found, return its value
		return itr->value;
	}
	else {
		std::cout << "Key "<< key <<  "not found in current file" << std::endl;
		for (auto it = deserialize_data.MemberBegin(); it != deserialize_data.MemberEnd(); ++it) {
			std::cout << "Available key: " << it->name.GetString() << std::endl;
		}
		return std::nullopt;
	}
}

bool JSONmanager::ResetReadStream(const std::string& new_filename) 
{
	if (ReadFilestream.is_open()) {
		ReadFilestream.close();
	}
	deserialize_data.SetNull();

	// open again
	ReadFilestream.open(new_filename, std::ios::in);
	assert(ReadFilestream.is_open() && "Failed to open file!");
	if (!ReadFilestream.is_open())
		throw std::runtime_error(std::format("JSON manager failed to open file %s", new_filename));

	// Check if the file is empty
	ReadFilestream.seekg(0, std::ios::end);
	if (ReadFilestream.tellg() == 0) {
		std::cerr << "Error: File is empty!" << std::endl;
	}
	ReadFilestream.seekg(0, std::ios::beg);  // Reset to the start of the file

	// Parse again
	rapidjson::IStreamWrapper isw(ReadFilestream);
	if (deserialize_data.ParseStream(isw).HasParseError()) {
		throw std::runtime_error(std::format("File has parse error %s", new_filename));
	}
	if (!deserialize_data.IsObject()) {
		std::cerr << "[Error] JSON root is not an object. filename provided: " << new_filename << std::endl;
	}
	return true;
}


JSONmanager::~JSONmanager() {
	if (ReadFilestream.is_open()) {
		ReadFilestream.close();
	}
	if (WriteFilestream.is_open()) {
		WriteFilestream.close();
	}
	deserialize_data.SetNull();
}



bool JSONmanager::OpenWriteStream(const std::string& outFilename)
{
	if (WriteFilestream.is_open()) {
		WriteFilestream.close();
	}

	// reset the stream
	m_NumObjectsStarted = 0;
	m_NumArraysStarted = 0;
	m_StringBuffer.Clear();
	m_pWriter = std::make_unique<rapidjson::PrettyWriter<rapidjson::StringBuffer>>(m_StringBuffer);

	WriteFilestream.open(outFilename, std::ios::out);
	if (!WriteFilestream.is_open())
		throw std::runtime_error(std::format("JSON manager failed to open Writefile %s", outFilename));
	assert(m_NumObjectsStarted == 0 && "stream has already been started.");
	if (m_NumObjectsStarted != 0)    return false;
	++m_NumObjectsStarted;
	return m_pWriter->StartObject();
}

bool JSONmanager::EndWriteStream()
{
	// Test if there's only one last object left
	assert(m_NumObjectsStarted == 1 && "There should only be one object open.");
	assert(m_NumArraysStarted == 0 && "Too many arrays started. Need to call EndArray().");
	if (m_NumObjectsStarted != 1 || m_NumArraysStarted != 0)    return false;
	m_pWriter->EndObject();
	WriteFilestream << m_StringBuffer.GetString();
	WriteFilestream.close();

	// reset the stream
	m_NumObjectsStarted = 0;
	m_NumArraysStarted = 0;
	m_StringBuffer.Clear();

	return true;
}

bool JSONmanager::StartNewObject(const std::string& key) 
{
	++m_NumObjectsStarted;
	if (!key.empty())    m_pWriter->Key(key.c_str());
	return m_pWriter->StartObject();
}

bool JSONmanager::EndObject() 
{
	assert(m_NumObjectsStarted > 1 && "EndObject() called too many times!");
	--m_NumObjectsStarted;
	return m_pWriter->EndObject();
}

bool JSONmanager::StartNewArray(const std::string& key) 
{
	++m_NumArraysStarted;
	m_pWriter->Key(key.c_str());
	return m_pWriter->StartArray();
}

bool JSONmanager::EndArray() 
{
	assert(m_NumArraysStarted > 0 && "EndArray() called too many times!");
	--m_NumArraysStarted;
	return m_pWriter->EndArray();
}