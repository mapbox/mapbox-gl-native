#include <mbgl/storage/asset_request_baton.hpp>
#include <mbgl/storage/asset_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <uv.h>

namespace mbgl {

AssetRequestBaton::AssetRequestBaton(AssetRequest *request_, const std::string &path_, uv_loop_t *loop)
    : threadId(uv_thread_self()),
      request(request_),
      path(path_) {

    asyncRun = mbgl::util::make_unique<uv::async>(loop,  [this]() {
        run(this);
    });

    asyncRun->send();
}

void AssetRequestBaton::cancel() {
    canceled = true;
}

void AssetRequestBaton::notify_error(AssetRequestBaton *ptr, const int code, const char *message) {
    assert(ptr->threadId == uv_thread_self());

    if (ptr->request && !ptr->canceled) {
        ptr->request->response = std::unique_ptr<Response>(new Response);
        ptr->request->response->code = code;
        ptr->request->response->message = message;
        ptr->request->notify();
    }
}

void AssetRequestBaton::cleanup(AssetRequestBaton *ptr) {
    assert(ptr->threadId == uv_thread_self());

    if (ptr->request) {
        ptr->request->ptr = nullptr;
    }

    delete ptr;
    ptr = nullptr;

    ptr->asyncRun.reset();
}

}
