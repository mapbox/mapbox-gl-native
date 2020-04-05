#pragma once

#include <uv.h>

#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>

namespace node_mbgl {
namespace util {

template <typename T>
class AsyncQueue {
public:
    AsyncQueue(uv_loop_t *loop, std::function<void(T &)> fn) : callback(std::move(fn)) {
        async.data = this;
        uv_async_init(loop, &async, [](uv_async_t* handle) {
            auto q = reinterpret_cast<AsyncQueue *>(handle->data);
            q->process();
        });
    }
    ~AsyncQueue() = default;

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
        uv_close(reinterpret_cast<uv_handle_t *>(&async),
                 [](uv_handle_t *handle) { delete reinterpret_cast<AsyncQueue *>(handle->data); });
    }

    void ref() { uv_ref(reinterpret_cast<uv_handle_t *>(&async)); }

    void unref() { uv_unref(reinterpret_cast<uv_handle_t *>(&async)); }

private:
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

} // namespace util
} // namespace node_mbgl
