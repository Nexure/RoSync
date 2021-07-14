#include "./sync_directory.h"
#include "./utils.h"

sync_directory::sync_directory(const std::string directory, std::shared_ptr<session_container> sessions_) 
	: tracker(file_tracker(directory, sessions_)), root_directory(directory), id(make_id()), sessions(sessions_)
{

}

sync_directory::~sync_directory()
{

}

std::string sync_directory::get_id() const
{
	return id;
}

std::string sync_directory::get_directory() const
{
	return root_directory;
}

const file_tracker& sync_directory::get_tracker() const
{
	return tracker;
}

