#pragma once

#include <iostream>
#include <shared_mutex>
#include <time.h>

// session timeout
constexpr auto SESSION_TIMEOUT = 60;

class session {
private:
	// session id
	const std::string id;

	// expires at
	time_t expires;

	// session mutex
	mutable std::shared_mutex session_lock;

public:
	// get the session id
	std::string get_id() const;

	// has the session expired
	bool has_expired() const;

	// update the expire time
	void refresh_session();

	session();
	~session();
};