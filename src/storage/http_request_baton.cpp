#include <mbgl/storage/http_request_baton.hpp>
#include <uv.h>

namespace mbgl {

HTTPRequestBaton::HTTPRequestBaton(const std::string &path_) : thread_id(uv_thread_self()), path(path_) {
}

HTTPRequestBaton::~HTTPRequestBaton() {
}

}
