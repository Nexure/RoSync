#include <algorithm>
#include <uwebsockets/App.h>
#include <vector>
#include <thread>
#include <future>

constexpr short PORT = 7777;

void handler(uWS::HttpResponse<false>* response, uWS::HttpRequest* request) {
	response->onData([=](auto data, auto is_final) {
		//std::cout << data << std::endl;
		std::cout << "is_final: " << is_final << std::endl;
	});

	response->onAborted([=]() {

	});
}

void start_webserver() {
	uWS::App server_instance;

	server_instance
		.get("/endpoint", [](auto res, auto req) {
			std::async(std::bind(handler, res, req));
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