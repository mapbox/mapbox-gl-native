#include <mbgl/platform/request.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/uv_detail.hpp>

using namespace mbgl::platform;

Request::Request(const std::string &url,
                 std::function<void(Response *)> callback,
                 std::shared_ptr<uv::loop> loop)
    : url(url),
      res(std::make_unique<Response>(callback)),
      cancelled(false),
      loop(loop) {
    if (loop) {
        // Add a check handle without a callback to keep the default loop running.
        // We don't have a real handler attached to the default loop right from the
        // beginning, because we're using asynchronous messaging to perform the actual
        // request in the request thread. Only after the request is complete, we
        // create an actual work request that is attached to the default loop.
        async = new uv_async_t();
        async->data = new std::unique_ptr<Response>();
        uv_async_init(**loop, async, complete);
    }
}

Request::~Request() {
}

void Request::complete() {
    if (loop) {
        // We're scheduling the response callback to be invoked in the event loop.
        // Since the Request object will be deleted before the callback is invoked,
        // we move over the Response object to be owned by the async handle.
        ((std::unique_ptr<Response> *)async->data)->swap(res);
        uv_async_send(async);
    } else {
        // We're calling the response callback immediately. We're currently on an
        // arbitrary thread, but that's okay.
        res->callback(res.get());
    }
}

void Request::complete(uv_async_t *async) {
    Response *res = static_cast<std::unique_ptr<Response> *>(async->data)->get();

    res->callback(res);

    // We need to remove our async handle again to allow the main event loop to exit.
    uv_close((uv_handle_t *)async, [](uv_handle_t *handle) {
        delete static_cast<std::unique_ptr<Response> *>(handle->data);
        delete (uv_async_t *)handle;
    });
}
