#include <mbgl/storage/request.hpp>

#include <mbgl/storage/response.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <cassert>
#include <functional>
#include <atomic>

namespace mbgl {

struct Request::Canceled { std::mutex mutex; bool confirmed = false; };

// Note: This requires that loop is running in the current thread (or not yet running).
Request::Request(const Resource &resource_, uv_loop_t *loop, Callback callback_)
    : async(std::make_unique<uv::async>(loop, [this] { notifyCallback(); })),
      callback(callback_),
      resource(resource_) {
}

// Called in the originating thread.
void Request::notifyCallback() {
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
        callback(*std::atomic_load(&response));
    }
}

Request::~Request() = default;

// Called in the FileSource thread.
void Request::notify(const std::shared_ptr<const Response> &response_) {
    assert(!std::atomic_load(&response));
    assert(response_);
    std::atomic_store(&response, response_);
    async->send();
}

// Called in the originating thread.
void Request::cancel() {
    assert(async);
    assert(!canceled);
    canceled = std::make_unique<Canceled>();
}


// Called in the FileSource thread.
// Will only ever be invoked after cancel() was called in the original requesting thread.
void Request::destruct() {
    assert(async);
    assert(canceled);
    std::unique_lock<std::mutex> lock(canceled->mutex);
    canceled->confirmed = true;
    async->send();
    // after this method returns, the FileSource thread has no knowledge of
    // this object anymore.
}

}
