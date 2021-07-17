#pragma once

#include <iostream>

namespace rosync::http {
	class Session {
		const std::string_view id_;

	public:
		explicit Session(const std::string id);
		~Session();
	};
}