#pragma once

#include <string>
#include <shared_mutex>

namespace rosync::http {
	enum class RequestType {
		Any,
		Get,
		Post,
		Patch,
		Put,
		Del
	};

	constexpr std::string request_type_to_string(const RequestType type) {
		switch (type) {
		case RequestType::Any: return "ANY";
		case RequestType::Get: return "GET";
		case RequestType::Post: return "POST";
		case RequestType::Patch: return "PATCH";
		case RequestType::Put: return "PUT";
		case RequestType::Del: return "DEL";
		default: return "UNKNOWN";
		}
	}

	class Request {
		std::string body_;
		
		explicit Request(std::string body);
	public:
		friend class RequestBuilder;
	};
	
	class RequestBuilder {
		std::string buffer_;
		std::shared_mutex lock_;
		std::condition_variable_any conditional_var_;
		std::atomic<bool> aborted_ = false;
		std::atomic<bool> finished_ = false;
	public:
		void append(std::string_view data, bool finished = false);
		bool await();
		void abort();
		[[nodiscard]] std::unique_ptr<Request> finish() const;
	};
} 