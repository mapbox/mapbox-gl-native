#include <mbgl/storage/request.hpp>

#include <mbgl/storage/response.hpp>

#include <mbgl/util/util.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/uv.hpp>

#include <uv.h>

#include <cassert>
#include <functional>

namespace mbgl {

struct Request::Canceled { std::mutex mutex; bool confirmed = false; };

// Note: This requires that loop is running in the current thread (or not yet running).
Request::Request(const Resource &resource_, uv_loop_t *loop, Callback callback_)
    : callback(callback_), resource(resource_) {
    // When there is no loop supplied (== nullptr), the callback will be fired in an arbitrary
    // thread (the thread notify() is called from) rather than kicking back to the calling thread.
    if (loop) {
        async = new uv_async_t;
        async->data = this;
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
        uv_async_init(loop, async, [](uv_async_t *a, int) { reinterpret_cast<Request *>(a->data)->notifyCallback(); });
#else
        uv_async_init(loop, async, [](uv_async_t *a) { reinterpret_cast<Request *>(a->data)->notifyCallback(); });
#endif
    }
}

// Called in the originating thread.
void Request::notifyCallback() {
    MBGL_VERIFY_THREAD(tid)
    if (!canceled) {
        invoke();
    } else {
        bool destroy = false;
        {
            std::unique_lock<std::mutex> lock(canceled->mutex);
            destroy = canceled->confirmed;
        }
        // Don't delete right way, because we have to unlock the mutex before deleting.
        if (destroy) {
            delete this;
        }
    }
}

void Request::invoke() {
    assert(response);
    // The user could supply a null pointer or empty std::function as a callback. In this case, we
    // still do the file request, but we don't need to deliver a result.
    if (callback) {
        callback(*response);
    }
    delete this;
}

Request::~Request() {
    if (async) {
        uv_close(reinterpret_cast<uv_handle_t*>(async), [](uv_handle_t* handle) {
            delete reinterpret_cast<uv_async_t*>(handle);
        });
    }
}

// Called in the FileSource thread.
void Request::notify(const std::shared_ptr<const Response> &response_) {
    assert(!response);
    response = response_;
    assert(response);

    if (async) {
        uv_async_send(async);
    } else {
        // This request is not cancelable. This means that the callback will be executed in an
        // arbitrary thread (== FileSource thread).
        invoke();
    }
}

// Called in the originating thread.
void Request::cancel() {
    MBGL_VERIFY_THREAD(tid)
    assert(async);
    assert(!canceled);
    canceled = util::make_unique<Canceled>();
}


// Called in the FileSource thread.
// Will only ever be invoked after cancel() was called in the original requesting thread.
void Request::destruct() {
    assert(async);
    assert(canceled);
    std::unique_lock<std::mutex> lock(canceled->mutex);
    canceled->confirmed = true;
    uv_async_send(async);
    // after this method returns, the FileSource thread has no knowledge of
    // this object anymore.
}

}
