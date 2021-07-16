#include <algorithm>
#include <uwebsockets/App.h>
#include <vector>
#include <thread>
#include <future>

#include <request.h>

constexpr short PORT = 7777;

void handler(uWS::HttpResponse<false>* response, uWS::HttpRequest* request) {

	rosync::http::RequestBuilder builder;
	
	response->onData([&](auto data, auto is_final) {
		builder.append(data, is_final);
	});
	
	response->onAborted([&]() {
		builder.abort();
		// TODO: clean up, although since builder is stack initialised - it'll be cleaned up.
	});
	
	// TODO: await here!
	if (!builder.await()) {
		auto result = builder.finish();
		// TODO: pass request to body after awaiting
	}
}

void start_webserver() {
	uWS::App server_instance;

	server_instance
		.post("/endpoint", [](auto res, auto req) {
			auto async_result = std::async([res, req] { return handler(res, req); });
		})
		.listen(PORT, [&](auto* token) {
			//std::scoped_lock lock(webserver_mutex);
			std::cout << "Thread " << std::this_thread::get_id() << std::endl;

			if (token) {
				std::cout << " listening on port " << PORT << std::endl;
			}
			else {
				std::cout << " failed to listen on port " << PORT << std::endl;
			}
		}).run();
}

int run(const size_t core_count = std::thread::hardware_concurrency()) {
	// initialize vector
	std::vector<std::thread*> threads(core_count);

	// create web-server based upon thread count
	std::transform(threads.begin(), threads.end(), threads.begin(), [&](std::thread*) {
		return new std::thread(start_webserver);
	});

	// wait for all threads, and delete before exiting
	std::for_each(threads.begin(), threads.end(), [](std::thread* t) {
		t->join();
		delete t;
	});

	return 0;
}

int main () {
	return run();
}