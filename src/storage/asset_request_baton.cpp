#include <mbgl/android/jni.hpp>
#include <mbgl/storage/asset_request_baton.hpp>
#include <mbgl/storage/asset_request.hpp>
#include <mbgl/storage/response.hpp>

#include <limits>
#include <boost/make_unique.hpp>

namespace mbgl {

AssetRequestBaton::AssetRequestBaton(AssetRequest *request_, const std::string &path_, uv_loop_t *loop)
    : thread_id(uv_thread_self()),
      request(request_),
      async_run(new uv_async_t()),
      path(path_) {

    uv_async_init(loop, async_run.get(), run);
    async_run->data = this;

    uv_async_send(async_run.get());
}

AssetRequestBaton::~AssetRequestBaton() {
    uv_close((uv_handle_t *)async_run.get(), nullptr);
}

void AssetRequestBaton::cancel() {
    canceled = true;
}

void AssetRequestBaton::notify_error(uv_async_t *async, const int code, const char *message) {
    AssetRequestBaton *ptr = (AssetRequestBaton *)async->data;
    assert(ptr->thread_id == uv_thread_self());

    if (ptr->request && !ptr->canceled) {
        ptr->request->response = std::unique_ptr<Response>(new Response);
        ptr->request->response->code = code;
        ptr->request->response->message = message;
        ptr->request->notify();
    }
}

void AssetRequestBaton::cleanup(uv_async_t *async) {
    AssetRequestBaton *ptr = (AssetRequestBaton *)async->data;
    assert(ptr->thread_id == uv_thread_self());

    if (ptr->request) {
        ptr->request->ptr = nullptr;
    }

    delete ptr;
}

}
