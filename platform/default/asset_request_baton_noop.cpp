#include <mbgl/storage/asset_request_baton.hpp>
#include <mbgl/storage/asset_request.hpp>
#include <mbgl/storage/response.hpp>

namespace mbgl {

void AssetRequestBaton::run(uv_async_t *async) {
    AssetRequestBaton *ptr = (AssetRequestBaton *)async->data;
    assert(ptr->thread_id == uv_thread_self());

    if (ptr->canceled || !ptr->request) {
        // Either the AssetRequest object has been destructed, or the
        // request was canceled.
        cleanup(async);
        return;
    }

    notify_error(async, 500, "Assets not implemented on this platform.");
    cleanup(async);
}

}
