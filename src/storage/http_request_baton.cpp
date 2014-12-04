#include <mbgl/storage/http_request_baton.hpp>
#include <uv.h>

namespace mbgl {

<<<<<<< HEAD
HTTPRequestBaton::HTTPRequestBaton(const std::string &path_) : threadId(uv_thread_self()), path(path_) {
=======
HTTPRequestBaton::HTTPRequestBaton(const std::string &path_) : thread_id(std::this_thread::get_id()), path(path_) {
>>>>>>> 57249ca32c7b0684be36f5195d4967e6517fe75b
}

HTTPRequestBaton::~HTTPRequestBaton() {
}

}
