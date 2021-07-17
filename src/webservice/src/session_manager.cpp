#include <session_manager.h>

namespace rosync::http {
	SessionManager::SessionManager()
	{
	}

	SessionManager::~SessionManager() {
	}

	std::shared_ptr<Session> SessionManager::create_session() {
		std::unique_lock lock(session_lock_);

		// TODO: implement
		auto id = make_id();
		auto session = std::make_unique<Session>(id);
		sessions_[id] = session;

		return session;
	}

	void SessionManager::destroy_session(std::string id) {
		std::unique_lock lock(session_lock_);
		sessions_.erase(id);
	}

	std::shared_ptr<Session> SessionManager::get_session(std::string id) {
		std::shared_lock lock(session_lock_);

		return std::shared_ptr<Session>();
	}
}