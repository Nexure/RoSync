#pragma once

#include "./session.h"

#include <shared_mutex>
#include <vector>

class session_container {
private:
	std::vector<std::shared_ptr<session>> sessions;

	mutable std::shared_mutex server_lock;
public:
	session_container();
	~session_container();

	// get a session
	bool get_session(std::string id, std::shared_ptr<session>* session);
	// create a client session, and return its id
	std::string create_session();
	// remove a client session
	bool remove_session(std::string id);
	// refreshes a sessions time-stamp
	bool refresh_session(std::string id);
};