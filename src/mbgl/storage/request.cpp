#include <mbgl/storage/default/request.hpp>

#include <mbgl/map/environment.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/util.hpp>

#include <cassert>
#include <functional>
#include <mutex>

namespace mbgl {

struct Request::Canceled { std::mutex mutex; bool confirmed = false; };

Request::Request(const Resource& resource_, std::thread::id tid_, uint32_t env_, Callback callback_)
    : callback(callback_), tid(tid_), resource(resource_), env(env_) {
}

void Request::notifyCallback() {
    if (tid != std::this_thread::get_id()) {
        Environment::postTask(tid, std::bind(&Request::notifyCallback, this));
        return;
    }

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
    // Only post the task if we have a valid thread::id, otherwise just
    // invoke on the callee thread.
    if (tid != std::thread::id() && tid != std::this_thread::get_id()) {
        Environment::postTask(tid, std::bind(&Request::invoke, this));
        return;
    }

    assert(response);
    // The user could supply a null pointer or empty std::function as a callback. In this case, we
    // still do the file request, but we don't need to deliver a result.
    if (callback) {
        callback(*response);
    }
    delete this;
}

// Called in the FileSource thread.
void Request::notify(const std::shared_ptr<const Response> &response_) {
    assert(!response);
    response = response_;
    assert(response);

    invoke();
}

// Called in the originating thread.
void Request::cancel() {
    MBGL_VERIFY_THREAD(tid)
    assert(!canceled);
    canceled = util::make_unique<Canceled>();
}


// Called in the FileSource thread.
// Will only ever be invoked after cancel() was called in the original requesting thread.
void Request::destruct() {
    assert(canceled);
    std::unique_lock<std::mutex> lock(canceled->mutex);
    canceled->confirmed = true;
    Environment::postTask(tid, std::bind(&Request::notifyCallback, this));
    // after this method returns, the FileSource thread has no knowledge of
    // this object anymore.
}

}
