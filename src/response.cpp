#include "response.h"

#define VALIDATE_CONTENT_TYPE(headers, content_type) \
	if (!headers.contains("Content-Type")) \
		headers["Content-Type"] = content_type

http_response::http_response() : status(status_code::NoContent)
{
}

http_response::http_response(status_code status_, header_map headers_) : status(status_), headers(headers_)
{
}

http_response::http_response(status_code status_, ::json json_, header_map headers_) : status(status_), headers(headers_), raw_body(json_.dump())
{
	// update the internal content-type if not present
	VALIDATE_CONTENT_TYPE(headers, "application/json");
}

http_response::http_response(status_code status_, std::string body, header_map headers_) : status(status_), headers(headers_), raw_body(body)
{
	// update the internal content-type if not existent
	VALIDATE_CONTENT_TYPE(headers, "text/plain");
}

http_response::~http_response()
{
}

status_code http_response::get_status() const
{
	// return copy of internal status
	return status;
}

header_map http_response::get_headers() const
{
	// return copy of headers
	return headers;
}

std::string http_response::get_body() const
{
	// return copy of body
	return raw_body;
}

void http_response::set_status(status_code status_)
{
	// update the status value
	status = status_;
}

void http_response::set_header(std::string header, std::string value)
{
	// update internal header
	headers[header] = value;
}

void http_response::set_headers(header_map headers_)
{
	// update all internal headers with headers from new header map
	for (auto it : headers_)
	{
		// set the value
		headers[it.first] = it.second;
	}
}

void http_response::json(nlohmann::json body)
{
	// set the respective content type
	headers["Content-Type"] = "application/json";

	// update the body
	raw_body = body.dump();
}

void http_response::body(std::string body, std::string content_type)
{
	// update the response content type
	headers["Content-Type"] = content_type;
	
	// update the internal body
	raw_body = body;
}

void http_response::write(uWS::HttpResponse<false>* response) const
{
	response->cork([=] 
		{
			// status message
			auto status_msg = std::format("{} {}", std::to_string(status), get_status_message(status));
			// write status
			response->writeStatus(status_msg);
			// write the headers
			for (auto header : headers)
			{
				response->writeHeader(header.first, header.second);
			}

			// write the body and end the response
			response->end(raw_body);
		});


} 
