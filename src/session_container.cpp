#include "./session_container.h"

session_container::session_container()
{
}

session_container::~session_container()
{
}

bool session_container::get_session(std::string id, std::shared_ptr<session>* session)
{
	std::shared_lock lock(server_lock);

	for (auto it : sessions)
	{
		if (it->get_id() == id) 
		{
			*session = it;
			return true;
		}
	}

	return false;
}

std::string session_container::create_session()
{
	std::unique_lock lock(server_lock);
	
	auto new_session = std::make_shared<session>();
	sessions.push_back(new_session);

	return new_session->get_id();
}

bool session_container::remove_session(std::string id)
{
	std::unique_lock lock(server_lock);
	auto it = sessions.begin();

	while (it != sessions.end())
	{
		if ((*it)->get_id() == id) 
		{
			sessions.erase(it);
			return true;
		}  
		else
		{
			it = ++it;
		}
	}

	return false;
}

bool session_container::refresh_session(std::string id)
{
	return false;
}
