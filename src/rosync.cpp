#include "./rosync.h"
#include "./utils.h"

rosync::rosync() : sessions(std::make_shared<session_container>()), sync_dir_manager(std::make_shared<sync_manager>(sessions))
{
}

rosync::~rosync() 
{
}


http_response rosync::get_health(shttp_request request)
{
	return http_response(status_code::OK);
}

http_response rosync::get_sync_dirs(shttp_request request)
{
	json response = json::array();

	for (auto it : sync_dir_manager->get_sync_directories())
	{
		response.push_back(json
			{
				{"id", it->get_id()},
				{"path", it->get_directory()}
			}
		);
	}

	return http_response(status_code::OK, response);
}

http_response rosync::get_sync_dirs_hashes(shttp_request request)
{
	return http_response();
}

http_response rosync::sync_dir(shttp_request request)
{
	auto json_body = request->get_json_body();
	auto directory = json_body["directory"].get<std::string>();
	std::shared_ptr<sync_directory> sync_directory;

	// BROKEN rn
	// replace the path dependent on the os
	// directory = sanitize_path(directory);

	// get the created directory
	if (!sync_dir_manager->add_directory(directory, &sync_directory))
	{
		throw status_exception::with_error_message(status_code::InternalServerError, "Failed to create sync directory");
	}

	return http_response(status_code::OK, json
		{
			{"id", sync_directory->get_id()},
			{"directory", directory},
			{"dirs", ""},
			{"files", ""}
		});
}

void rosync::map_endpoint(uWS::App& http_server, http_method method, std::string endpoint, web_handler function)
{
	auto request_call = [=](uws_response res, uws_request req, shttp_request request)
	{
#define CAN_WRITE(x) if (!request->get_token().is_canceled()) x

		try
		{
			auto response = function(request);
			CAN_WRITE(response.write(res));
		}
		catch (status_exception& status)
		{
			CAN_WRITE(status.write_response(res));
		}
		catch (nlohmann::json::exception& e)
		{
			auto response = status_exception::with_error_message(status_code::InternalServerError, "Invalid json data passed");
			CAN_WRITE(response.write_response(res));
		}
		catch (std::exception& e)
		{
			auto response = status_exception::with_error_message(status_code::InternalServerError, e.what());
			CAN_WRITE(response.write_response(res));
		}

#undef CAN_WRITE
	};

	auto request_proxy = [=](uws_response res, uws_request req)
	{
		concurrency::cancellation_token_source cts;
		auto token = cts.get_token();
		auto request = std::make_shared<http_request>(req, res, token);

		auto task = concurrency::create_task([=]
			{
				request->await_body();

				if (token.is_canceled())
				{
					return;
				}

				request_call(res, req, request);
			}, token
		);

		res->onAborted([=] {
			cts.cancel();
		});

	};

	switch (method) {
	case http_method::ANY:
		http_server.any(endpoint, request_proxy);
		break;
	case http_method::GET:
		http_server.get(endpoint, request_proxy);
		break;
	case http_method::POST:
		http_server.post(endpoint, request_proxy);
		break;
	case http_method::PATCH:
		http_server.any(endpoint, request_proxy);
		break;
	case http_method::PUT:
		http_server.put(endpoint, request_proxy);
		break;
	case http_method::DEL:
		http_server.del(endpoint, request_proxy);
		break;
	default:
		throw std::runtime_error("invalid endpoint method passed");
	}
}

void rosync::register_endpoints(uWS::App& http_server)
{
// generic method to make mapping endpoints less repetitive and long
#define MAP_ENDPOINT(method, endpoint, func_name) \
	map_endpoint(http_server, method, endpoint, std::bind(&rosync::func_name, this, std::placeholders::_1))

	// endpoint mapping to functions
	MAP_ENDPOINT(http_method::GET, "/health", get_health);
	// endpoint to get sync directories
	MAP_ENDPOINT(http_method::GET, "/syncing", get_sync_dirs);
	// endpoint to start syncing directory
	MAP_ENDPOINT(http_method::PUT, "/syncing", sync_dir);
	// endpoint to get sync dir hashes
	MAP_ENDPOINT(http_method::GET, "/syncing/:id", get_sync_dirs_hashes);


	// generic http endpoints
	http_server
		.get("/*", [](auto* res, auto* req)
			{
				http_response response(status_code::NotFound, json
					{
						{"code", "404"},
						{"message", "Not Found"}
					});

				response.write(res);
			});
	
#undef MAP_ENDPOINT
}

void rosync::start_webserver()
{
	// create server instances
	uWS::App server_instance;
	
	// register the endpoints
	register_endpoints(server_instance);

	// start up the instance to listen
	server_instance
		.listen(PORT, [&](auto* token)
			{
				std::scoped_lock lock(webserver_mutex);
				std::cout << "Thread " << std::this_thread::get_id();

				if (token) 
				{
					std::cout << " listening on port " << PORT << std::endl;
				}
				else
				{
					std::cout << " failed to listen on port" << PORT << std::endl;
				}
			})
		.run();
}


#ifdef USE_TASK_POOL
void rosync::run()
{
	// create array of server tasks at the system concurrency limit
	std::vector<concurrency::task<void>> tasks;

	// create web-server based on takss
	for (auto i = 0; i < std::thread::hardware_concurrency(); i++)
	{
		auto task = concurrency::create_task(std::bind(&rosync::start_webserver, this));
		tasks.push_back(task);
	}

	// wait for all server threads to stop before exiting the program
	concurrency::when_all(tasks.begin(), tasks.end()).get();
}
#else
void rosync::run() 
{
	// create array of server threads at the system concurrency limit
	std::vector<std::thread*> threads(std::thread::hardware_concurrency());

	// create web-serverb based on threads
	std::transform(threads.begin(), threads.end(), threads.begin(), [&](std::thread*) {
		return new std::thread(std::bind(&rosync::start_webserver, this));
	});

	// wait for all server threads to stop before exiting the program
	std::for_each(threads.begin(), threads.end(), [](std::thread* t) {
		t->join();
		delete t;
	});
}
#endif
