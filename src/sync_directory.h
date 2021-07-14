#pragma once

#include "./session_container.h"
#include "./file_tracker.h"

#include <iostream>
#include <shared_mutex>

class sync_directory {
private:
	// internal identifiers
	const std::string id;
	const std::string root_directory;

	// managers
	const std::shared_ptr<session_container> sessions;
	const file_tracker tracker;

	// lock for modifying 
	mutable std::shared_mutex sync_lock;

public:
	sync_directory(const std::string directory, const std::shared_ptr<session_container> sessions);
	~sync_directory();

	std::string get_id() const;
	std::string get_directory() const;

	const file_tracker& get_tracker() const;
};