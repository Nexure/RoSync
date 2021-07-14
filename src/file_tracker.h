#pragma once

#include "./session_container.h"

#include <efsw/efsw.hpp>

#include <iostream>
#include <shared_mutex>
#include <ppltasks.h>
#include <filesystem>
#include <unordered_map>


// a tracked file
struct tracked_file {
	bool is_directory;
	uint32_t hash;
};

// the tracking state
enum class tracking_state {
	STOPPED,
	STARTED
};

// the main file tracker
class file_tracker {
private:

	// the file trackers root directory
	const std::string root_directory;

	// session container to noitfy clients
	const std::shared_ptr<session_container> sessions;

	// a map of the filter trackers directory
	std::unordered_map<std::string, tracked_file> file_map;
	
	// state of the file tracker
	tracking_state state;

	// file map locking
	mutable std::shared_mutex file_mutex;

	// fill the hash map
	void fill_file_map();

	concurrency::task<uint32_t> hash_file(std::string path);

	// file exists
	bool file_exists(std::string file);

	// directory exists
	bool dir_exists(std::string dir);

public:
	file_tracker(std::string directory, std::shared_ptr<session_container> sessions);
	~file_tracker();

	bool add_tracked_file(std::string file);
	bool remove_tracked_file(std::string file);
	
	tracked_file get_tracked_file(std::string file);
	const tracking_state get_tracking_state() const;

	std::vector<std::string> get_directories();
	std::unordered_map<std::string, uint32_t> get_files();

	void start_tracking();
	void stop_tracking();
};