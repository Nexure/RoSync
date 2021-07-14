#pragma once

#include "./sync_directory.h"

#include <iostream>
#include <shared_mutex>
#include <vector>


class sync_manager {
private:
	std::vector<std::shared_ptr<sync_directory>> sync_directories;

	// session storage
	const std::shared_ptr<session_container> sessions;

	// lock for sync directory
	mutable std::shared_mutex sync_lock;

public:
	sync_manager(const std::shared_ptr<session_container> sessions);
	~sync_manager();

	// a sync directory
	bool add_directory(const std::string directory, std::shared_ptr<sync_directory>* id = nullptr);
	
	// remove a directory
	void remove_directory(const std::string id);

	// directory getters
	const std::vector<std::shared_ptr<sync_directory>> get_sync_directories();
	bool get_directory_by_path(const std::string path, std::shared_ptr<sync_directory>* directory = nullptr) const;
	bool get_directory_by_id(const std::string id, std::shared_ptr<sync_directory>* directory = nullptr) const;
};