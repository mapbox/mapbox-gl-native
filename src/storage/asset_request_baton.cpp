#include <mbgl/storage/asset_request_baton.hpp>
#include <mbgl/storage/asset_request.hpp>
#include <mbgl/storage/response.hpp>

namespace mbgl {

AssetRequestBaton::AssetRequestBaton(AssetRequest *request_, const std::string &path_, uv_loop_t *loop)
    : thread_id(uv_thread_self()),
      request(request_),
      async_run(new uv_async_t()),
      path(path_) {

// Iron out the differences between libuv 0.10 and 0.11
#ifdef UV_ASYNC_CALLBACK
#error Cannot overwrite UV_ASYNC_CALLBACK
#endif
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
#define UV_ASYNC_CALLBACK(name) [](uv_async_t *a, int) { return AssetRequestBaton::name(a); }
#else
#define UV_ASYNC_CALLBACK(name) name
#endif

    uv_async_init(loop, async_run.get(), UV_ASYNC_CALLBACK(run));
    async_run->data = this;

#undef UV_ASYNC_CALLBACK

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
