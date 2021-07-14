#pragma once

#include "./request.h"
#include "./response.h"

#include "./session_container.h"
#include "./sync_manager.h"
#include "./status_exception.h"

#include <ppltasks.h>
#include <uwebsockets/App.h>

const short PORT = 7777;

using shttp_request = std::shared_ptr<http_request>;
using web_handler = std::function<http_response(shttp_request)>;

class rosync {
private:
	// constants
	const std::shared_ptr<session_container> sessions;
	const std::shared_ptr<sync_manager> sync_dir_manager;
	
	// endpoint handlers
	// get the server's health
	http_response get_health(shttp_request request);
	// get all syncing dirs
	http_response get_sync_dirs(shttp_request request);
	// get syncing dir hashes
	http_response get_sync_dirs_hashes(shttp_request request);
	// add syncing dir
	http_response sync_dir(shttp_request request);

	// used to register the endpoint handlers
	void register_endpoints(uWS::App& app);
	void map_endpoint(uWS::App& app, http_method method, std::string endpoint, web_handler handler);
	
	// start a single web-server (multiple instances can be created)
	void start_webserver();

	mutable std::mutex webserver_mutex;
public:
	rosync();
	~rosync();

	void run();
};