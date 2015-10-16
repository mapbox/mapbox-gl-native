#include <mbgl/storage/request.hpp>

#include <mbgl/storage/response.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <cassert>
#include <functional>
#include <memory>

namespace mbgl {

// Note: This requires that loop is running in the current thread (or not yet running).
Request::Request(const Resource &resource_, uv_loop_t *loop, Callback callback_)
    : async(std::make_unique<uv::async>(loop, [this] { notifyCallback(); })),
      callback(callback_),
      resource(resource_) {
}

// Called in the originating thread.
void Request::notifyCallback() {
    std::unique_lock<std::mutex> lock(mtx);
    if (!canceled) {
        // Move the response object out so that we can't accidentally notify twice.
        auto res = std::move(response);
        assert(!response);
        // Unlock before, since callbacks may call cancel, which also locks this mutex.
        lock.unlock();
        // The user could supply a null pointer or empty std::function as a callback. In this case, we
        // still do the file request, but we don't need to deliver a result.
        // Similarly, two consecutive updates could trigger two notifyCallbacks, so we need to make
        // sure
        if (callback && res) {
            callback(*res);
        }
    } else {
        // Don't delete right way, because we have to unlock the mutex before deleting.
        if (confirmed) {
            lock.unlock();
            delete this;
        }
    }
}

Request::~Request() = default;

// Called in the FileSource thread.
void Request::notify(const std::shared_ptr<const Response> &response_) {
    std::lock_guard<std::mutex> lock(mtx);
    assert(response_);
    response = response_;
    async->send();
}

// Called in the originating thread.
void Request::cancel() {
    std::lock_guard<std::mutex> lock(mtx);
    assert(!canceled);
    canceled = true;
}


// Called in the FileSource thread.
// Will only ever be invoked after cancel() was called in the original requesting thread.
void Request::destruct() {
    std::lock_guard<std::mutex> lock(mtx);
    assert(canceled);
    confirmed = true;
    // We need to extend the lock until after the async has been sent, otherwise the requesting
    // thread could destroy the async while this call is still in progress.
    async->send();
    // After this method returns, the FileSource thread has no knowledge of
    // this object anymore.
}

}
