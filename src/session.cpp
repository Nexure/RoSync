#include "session.h"
#include "utils.h"


session::session() : id(make_id())
{
	refresh_session();
}

session::~session() 
{
}

std::string session::get_id() const 
{
	return id;
}

bool session::has_expired() const
{
	std::shared_lock lock(session_lock);
	return expires > time(nullptr);
}

void session::refresh_session()
{
	std::unique_lock lock(session_lock);
	expires = time(nullptr) + SESSION_TIMEOUT;
}
