#include <mbgl/storage/http_request_baton.hpp>
#include <uv.h>

namespace mbgl {

HTTPRequestBaton::HTTPRequestBaton(const std::string &path_) : thread_id(std::this_thread::get_id()), path(path_) {
}

HTTPRequestBaton::~HTTPRequestBaton() {
}

}
