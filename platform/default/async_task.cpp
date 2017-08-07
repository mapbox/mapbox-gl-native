#include <mbgl/util/async_task.hpp>

#include <mbgl/util/run_loop.hpp>

#include <atomic>
#include <functional>

#include <uv.h>

namespace mbgl {
namespace util {

class AsyncTask::Impl {
public:
    Impl(std::function<void()>&& fn)
        : async(new uv_async_t),
          task(std::move(fn)) {

        auto* loop = reinterpret_cast<uv_loop_t*>(RunLoop::getLoopHandle());
        if (uv_async_init(loop, async, asyncCallback) != 0) {
            throw std::runtime_error("Failed to initialize async.");
        }

        handle()->data = this;
        uv_unref(handle());
    }

    ~Impl() {
        uv_close(handle(), [](uv_handle_t* h) {
            delete reinterpret_cast<uv_async_t*>(h);
        });
    }

    void maySend() {
        // uv_async_send will do the call coalescing for us.
        if (uv_async_send(async) != 0) {
            throw std::runtime_error("Failed to async send.");
        }
    }

private:
    static void asyncCallback(uv_async_t* handle) {
        reinterpret_cast<Impl*>(handle->data)->task();
    }

    uv_handle_t* handle() {
        return reinterpret_cast<uv_handle_t*>(async);
    }

    uv_async_t* async;

    std::function<void()> task;
};

AsyncTask::AsyncTask(std::function<void()>&& fn)
    : impl(std::make_unique<Impl>(std::move(fn))) {
}

AsyncTask::~AsyncTask() = default;

void AsyncTask::send() {
    impl->maySend();
}

} // namespace util
} // namespace mbgl
