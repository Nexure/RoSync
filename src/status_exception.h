#pragma once
// a lot of this should probably be implemented into response

#include "./status_code.h"
#include "./response.h"

#include <nlohmann/json.hpp>

#include <exception>
#include <unordered_map>

using header_map = std::unordered_map<std::string, std::string>;
using json = nlohmann::json;

class status_exception : public std::exception
{
private:
	const http_response response;

public:
	// status response
	status_exception(status_code status, header_map headers = {}) : response(status, headers) {}
	// raw response
	status_exception(status_code status, std::string body, header_map headers = {}) : response(status, body, headers) {}
	// json response 
	status_exception(status_code status, json json, header_map headers = {}) : response(status, json, headers) {}

	status_code get_status() const
	{
		return response.get_status();
	}

	header_map get_headers() const
	{
		return response.get_headers();
	}

	std::string get_body() const
	{
		return response.get_body();
	}

	http_response get_response() const
	{
		return response;
	}

	void write_response(uWS::HttpResponse<false>* response_) const
	{
		response.write(response_);
	}

	static status_exception with_error_message(status_code status, std::string message)
	{
		return status_exception(status, json
			{
				{"code", status},
				{"message", message}
			}
		);
	}
};