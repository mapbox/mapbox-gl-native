#include <mbgl/storage/file_request.hpp>
#include <mbgl/storage/file_request_baton.hpp>
#include <mbgl/storage/response.hpp>

#include <uv.h>

#include <cassert>

#include <unistd.h>

namespace mbgl {

FileRequest::FileRequest(const std::string &path_, uv_loop_t *loop)
    : BaseRequest(path_), ptr(new FileRequestBaton(this, path, loop)) {
}

void FileRequest::cancel() {
    assert(thread_id == std::this_thread::get_id());

    if (ptr) {
        ptr->cancel();

        // When deleting a FileRequest object with a uv_fs_* call is in progress, we are making sure
        // that the callback doesn't accidentally reference this object again.
        ptr->request = nullptr;
        ptr = nullptr;
    }

    notify();
}

FileRequest::~FileRequest() {
    assert(thread_id == std::this_thread::get_id());
    cancel();
}

}
