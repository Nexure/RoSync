#include "./request.h"
#include "./status_exception.h"


http_request::http_request(uws_request request_, uws_response response, concurrency::cancellation_token token_) : request(request_), token(token_)
{
	response->onData([&](std::string_view buffer, bool finished)
		{
			printf("Testing: %d\n", finished);

			if (buffer.length() > 0)
			{
				body << std::string(buffer.begin(), buffer.end());
				std::cout << "called \"" << buffer << "\" finished: " << finished << std::endl;
			}

			if (finished)
			{
				body_await.notify_all();
			}
		}
	);
}

http_request::~http_request()
{
}

concurrency::cancellation_token http_request::get_token() const
{
	return token;
}

void http_request::await_body()
{
	std::mutex body_mutex;
	std::unique_lock<std::mutex> lock(body_mutex);
	body_await.wait(lock);
}

const json http_request::get_json_body()
{
	auto body = get_body();
	if (!body.length())
	{
		throw status_exception::with_error_message(status_code::UnsupportedMediaType, "No data passed, json required");
	}

	try
	{
		return json::parse(body);
	} 
	catch (std::exception& e)
	{
		printf("error: %s\n", e.what());
		throw status_exception::with_error_message(status_code::UnsupportedMediaType, "Unable to parse json, please pass valid json");
	}
}

const std::string_view http_request::get_query() const
{
	return request->getQuery();
}

const std::string_view http_request::get_url() const
{
	return request->getUrl();
}

const std::string_view http_request::get_param(uint32_t index) const
{
	return request->getParameter(index);
}

const std::string_view http_request::get_header(const char* name) const
{
	
	return request->getHeader(name);
}

std::string http_request::get_body()
{
	return body.str();
}
