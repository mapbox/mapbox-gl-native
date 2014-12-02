#include <mbgl/storage/asset_request.hpp>
#include <mbgl/storage/asset_request_baton.hpp>
#include <mbgl/storage/response.hpp>

#include <uv.h>

#include <cassert>

#include <unistd.h>

namespace mbgl {

AssetRequest::AssetRequest(const std::string &path_, uv_loop_t *loop)
    : BaseRequest(path_), ptr(new AssetRequestBaton(this, path, loop)) {
}

void AssetRequest::cancel() {
    assert(thread_id == uv_thread_self());

    if (ptr) {
        ptr->cancel();

        // When deleting a AssetRequest object with a uv_fs_* call is in progress, we are making sure
        // that the callback doesn't accidentally reference this object again.
        ptr->request = nullptr;
        ptr = nullptr;
    }

    notify();
}

AssetRequest::~AssetRequest() {
    assert(thread_id == uv_thread_self());
    cancel();
}

}
