#include "./file_tracker.h"
#include "./status_exception.h"

#include <fstream>
#include <semaphore>

file_tracker::file_tracker(std::string directory_, std::shared_ptr<session_container> sessions_) : root_directory(directory_), sessions(sessions_)
{
	if (!dir_exists(root_directory))
	{
		throw status_exception::with_error_message(status_code::NotFound, "Sync directory does not exist");
	}

	state = tracking_state::STARTED;
	fill_file_map();
}

file_tracker::~file_tracker()
{
}


void file_tracker::fill_file_map()
{
	std::unique_lock lock(file_mutex);
	file_map.clear();

	// don't hash if the folder doesnt exist
	if (!dir_exists(root_directory))
	{
		return;
	}

	std::vector<concurrency::task<void>> tasks;
	std::counting_semaphore<4> semaphore(4);

	for (auto& it : std::filesystem::recursive_directory_iterator(root_directory))
	{
		tasks.push_back(concurrency::create_task([&, it]
			{
				semaphore.acquire();

				auto path = it.path();
				auto relative_path = path.lexically_relative(root_directory);
				tracked_file file;

				file.is_directory = it.is_directory();
				file.hash = !file.is_directory ? hash_file(path.string()).get() : 0;


				printf("file: %s, hash: %x\n", relative_path.string().c_str(), file.hash);

				file_map[relative_path.string()] = file;
				semaphore.release();
			}
		));
	}

	concurrency::when_all(tasks.begin(), tasks.end()).get();
}

concurrency::task<uint32_t> file_tracker::hash_file(std::string path)
{
	return concurrency::create_task([=]() -> uint32_t
		{
			std::ifstream file(path, std::ios::binary);
			if (!file.is_open())
			{
				return 0;
			}

			uint32_t hash = 0;
			uint8_t buff[1024];

			while (!file.eof())
			{
				file.read(reinterpret_cast<char*>(buff), 1024);
				hash = crc32(hash, buff, file.gcount());
			}

			return hash;
		}
	);
}

bool file_tracker::file_exists(std::string file)
{
	return std::filesystem::exists(file) && std::filesystem::is_regular_file(file);
}

bool file_tracker::dir_exists(std::string directory)
{
	return std::filesystem::exists(directory) && std::filesystem::is_directory(directory);
}

bool file_tracker::add_tracked_file(std::string file)
{
	return false;
}

bool file_tracker::remove_tracked_file(std::string file)
{
	return false;
}

tracked_file file_tracker::get_tracked_file(std::string file)
{
	return tracked_file();
}

const tracking_state file_tracker::get_tracking_state() const
{
	return state;
}