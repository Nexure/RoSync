#include <request.h>

namespace rosync::http {
	Request::Request(std::string body)
		: body_(std::move(body)) {	}
	
	void RequestBuilder::append(const std::string_view data, const bool finished) {
		std::unique_lock lock(lock_);
		buffer_ += std::string{ data };

		if (finished) {
			finished_ = true;
			conditional_var_.notify_all();
		}
	}

	bool RequestBuilder::await() {
		std::unique_lock lock(lock_);

		if (finished_) {
			return !aborted_;
		}

		// only wait if we haven't already finished
		conditional_var_.wait(lock);

		return !aborted_;
	}

	void RequestBuilder::abort() {
		aborted_ = true;
		conditional_var_.notify_all();
	}
	
	std::unique_ptr<Request> RequestBuilder::finish() const {
		return std::unique_ptr<Request>(new Request(buffer_));
	}
}