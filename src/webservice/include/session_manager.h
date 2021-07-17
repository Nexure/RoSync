#include <session.h>

#include <memory>
#include <unordered_map>
#include <shared_mutex>

namespace rosync::http {
	class SessionManager {
		const std::shared_mutex session_lock_;
		std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;

	public:
		SessionManager();
		~SessionManager();
		
		std::shared_ptr<Session> create_session();
		void destroy_session(std::string id);
		
		std::shared_ptr<Session> get_session(std::string id);
	};
}