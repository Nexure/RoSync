#pragma once

#include <string>

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

	
}
