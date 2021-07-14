#include "sync_manager.h"

sync_manager::sync_manager(const std::shared_ptr<session_container> sessions_) : sessions(sessions_) 
{
}

sync_manager::~sync_manager() 
{ 
}

bool sync_manager::add_directory(const std::string directory, std::shared_ptr<sync_directory>* sync_dir)
{
	if (get_directory_by_path(directory))
	{
		return false;
	}
	
	std::unique_lock lock(sync_lock);
	std::shared_ptr<sync_directory> new_sync_dir = std::make_shared<sync_directory>(directory, sessions);
	sync_directories.push_back(new_sync_dir);

	if (sync_dir != nullptr)
	{
		*sync_dir = new_sync_dir;
	}

	return true;
}

void sync_manager::remove_directory(const std::string id)
{
	std::unique_lock lock(sync_lock);
	auto it = sync_directories.begin();

	while (it != sync_directories.end())
	{
		if ((*it)->get_id() == id) 
		{
			it = sync_directories.erase(it);
		}
		else
		{
			++it;
		}
	}
}

const std::vector<std::shared_ptr<sync_directory>> sync_manager::get_sync_directories()
{
	std::shared_lock lock(sync_lock);
	std::vector<std::shared_ptr<sync_directory>> sync_dirs;

	for (auto& it : sync_directories) 
	{
		sync_dirs.push_back(it);
	}

	return sync_dirs;
}

bool sync_manager::get_directory_by_path(const std::string path, std::shared_ptr<sync_directory>* directory) const
{
	std::shared_lock lock(sync_lock);

	for (auto& it : sync_directories) 
	{
		if (it->get_directory() == path) 
		{
			if (directory != nullptr)
			{
				*directory = it;
			}

			return true;
		}
	}

	return false;
}

bool sync_manager::get_directory_by_id(const std::string id, std::shared_ptr<sync_directory>* directory) const
{
	std::shared_lock lock(sync_lock);

	for (auto& it : sync_directories)
	{
		if (it->get_id() == id) 
		{
			if (directory != nullptr)
			{
				*directory = it;
			}

			return true;
		}
	}

	return false;
}