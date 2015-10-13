#pragma once

#include <uv.h>

#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <string>

#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
#define UV_ASYNC_PARAMS(handle) uv_async_t *handle, int
#else
#define UV_ASYNC_PARAMS(handle) uv_async_t *handle
#endif

namespace node_mbgl {
namespace util {

template <typename T>
class AsyncQueue {
public:
    AsyncQueue(uv_loop_t *loop, std::function<void(T &)> fn) :
          callback(fn) {
        async.data = this;
        uv_async_init(loop, &async, [](UV_ASYNC_PARAMS(handle)) {
            auto q = reinterpret_cast<AsyncQueue *>(handle->data);
            q->process();
        });
    }

    void send(T &&data) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::make_unique<T>(std::move(data)));
        }
        uv_async_send(&async);
    }

    void send(std::unique_ptr<T> data) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(data));
        }
        uv_async_send(&async);
    }

    void stop() {
        uv_close((uv_handle_t *)&async, [](uv_handle_t *handle) {
            delete reinterpret_cast<AsyncQueue *>(handle->data);
        });
    }

    void ref() {
        uv_ref((uv_handle_t *)&async);
    }

    void unref() {
        uv_unref((uv_handle_t *)&async);
    }

private:
    ~AsyncQueue() {
    }

    void process() {
        std::unique_ptr<T> item;
        while (true) {
            mutex.lock();
            if (queue.empty()) {
                mutex.unlock();
                break;
            }
            item = std::move(queue.front());
            queue.pop();
            mutex.unlock();
            callback(*item);
        }
    }

private:
    std::mutex mutex;
    uv_async_t async;
    std::queue<std::unique_ptr<T>> queue;
    std::function<void(T &)> callback;
};

}
}
