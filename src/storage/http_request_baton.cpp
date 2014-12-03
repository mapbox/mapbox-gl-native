#include <mbgl/storage/http_request_baton.hpp>
#include <uv.h>

namespace mbgl {

HTTPRequestBaton::HTTPRequestBaton(const std::string &path_) : threadId(uv_thread_self()), path(path_) {
}

HTTPRequestBaton::~HTTPRequestBaton() {
}

}
