/**
 * 
 */

#pragma once

#include "status_code.h"

namespace rosync::http {
	using header_map = std::unordered_map<std::string, std::string>;
	
	/**
	 * HTTP response object.
	 */
	struct Response {
		status_code code; // response status code.
		header_map headers; // response headers.
		const std::string body; // response raw body.
	};
}
