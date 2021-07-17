#include <session.h>

namespace rosync::http {
    Session::Session(const std::string id) : id_(std::move(id)) { }
    Session::~Session() { }


}