// this is a response wrapper, because uWebsockets has a terrible class for http responses

#pragma once

#include "./status_code.h"

#include <nlohmann/json.hpp>
#include <uwebsockets/HttpContext.h>
#include <uwebsockets/HttpResponse.h>

#include <iostream>
#include <unordered_map>

using json = nlohmann::json;
using header_map = std::unordered_map<std::string, std::string>;

class http_response 
{
private:
	// response status
	status_code status;
	// response headers
	header_map headers;
	// response body
	std::string raw_body;

public:
	http_response();
	// status response
	http_response(status_code status, header_map headers = {});
	// json response
	http_response(status_code status, json json, header_map headers = {});
	// text response
	http_response(status_code status, std::string body, header_map headers = {});
	~http_response(); 

	// getters
	// get status
	status_code get_status() const;
	// get headers
	header_map get_headers() const;
	// get body
	std::string get_body() const;

	// update response
	void set_status(status_code status);
	void set_header(std::string header, std::string value);
	void set_headers(header_map headers);

	// set the response body
	void json(nlohmann::json body);
	void body(std::string body, std::string content_type = "text/plain");
	
	// write the response to uWS
	void write(uWS::HttpResponse<false>* response) const;
};