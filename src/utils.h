#pragma once

#include <iostream>
#include <sstream>
#include <mutex>
#include <random>

// generate a unique identifier
static std::string make_id()
{
	const char* id_chars = "0123456789abcdef";

	static std::random_device random_device;
	static std::mt19937 character_random(random_device());

	std::string id;
	std::uniform_int_distribution<int> distribution(0, 15);

	for (auto i = 0; i < 8; i++)
	{
		id += id_chars[distribution(character_random)];
	}

	return id;
}

// sanitize the path to use the correct directory format based upon the file system
static std::string sanitize_path(std::string path)
{
#ifdef _WIN32 
	return path.replace(path.begin(), path.end(), "/", "\\");
#else
	return path.replace(path.begin(), path.end(), "\\", "/");
#endif
}

// we should never allow going up a directory, it can be potentially dangerous
static bool is_path_safe(std::string path)
{
	return path.find("..") != std::string::npos;
}


// Simple implementation of an atomic stream
class atomic_stream
{
private:
	std::stringstream ss;
	std::mutex lock_;
public:
	atomic_stream() : ss() { }

	template <typename T>
	atomic_stream& operator<<(T const& t)
	{
		std::lock_guard lock(lock_);
		ss << t;
		return *this;
	}

	std::string str()
	{
		std::lock_guard lock(lock_);
		return ss.str();
	}
};