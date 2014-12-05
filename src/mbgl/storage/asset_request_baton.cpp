#include <mbgl/storage/asset_request_baton.hpp>
#include <mbgl/storage/asset_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/uv_detail.hpp>

namespace mbgl {

AssetRequestBaton::AssetRequestBaton(AssetRequest *request_, const std::string &path_, uv_loop_t *loop)
    : threadId(std::this_thread::get_id()),
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

void AssetRequestBaton::notifyError(AssetRequestBaton *ptr, const int code, const char *message) {
    assert(std::this_thread::get_id() == ptr->threadId);

    if (ptr->request && !ptr->canceled) {
        ptr->request->response = std::unique_ptr<Response>(new Response);
        ptr->request->response->code = code;
        ptr->request->response->message = message;
        ptr->request->notify();
    }
}

void AssetRequestBaton::notifySuccess(AssetRequestBaton *ptr, const std::string body) {
assert(std::this_thread::get_id() == ptr->threadId);

    if (ptr->request && !ptr->canceled) {
        ptr->request->response = std::unique_ptr<Response>(new Response);
        ptr->request->response->code = 200;
        ptr->request->response->data = body;
        ptr->request->notify();
    }
}

void AssetRequestBaton::cleanup(AssetRequestBaton *ptr) {
    assert(std::this_thread::get_id() == ptr->threadId);

    if (ptr->request) {
        ptr->request->ptr = nullptr;
    }

    ptr->asyncRun.reset();

    delete ptr;
    ptr = nullptr;
}

}
