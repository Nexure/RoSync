#pragma once

#include "./utils.h"

#include <nlohmann/json.hpp>
#include <uwebsockets/HttpContext.h>
#include <uwebsockets/HttpResponse.h>

#include <iostream>
#include <unordered_map>
#include <ppltasks.h>

// header map
using header_map = std::unordered_map<std::string, std::string>;
using json = nlohmann::json;

// types to make uws request/responses easier
using uws_request = uWS::HttpRequest*;
using uws_response = uWS::HttpResponse<false>*;

enum class http_method {
	ANY,
	GET,
	POST,
	PATCH,
	PUT,
	DEL
};

class http_request 
{
private:
	concurrency::cancellation_token token;
	const uws_request request;

	std::condition_variable body_await;
	atomic_stream body;

public:
	http_request(uws_request request, uws_response response, concurrency::cancellation_token token);
	~http_request();

	concurrency::cancellation_token get_token() const;

	void await_body();
	std::string get_body();
	const json get_json_body();

	
	const std::string_view get_query() const;
	const std::string_view get_url() const;
	const std::string_view get_param(uint32_t index) const;
	const std::string_view get_header(const char* name) const;
};