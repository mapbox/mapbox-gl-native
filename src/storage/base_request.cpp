#include <mbgl/storage/base_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/request.hpp>

#include <uv.h>

#include <cassert>

namespace mbgl {

template <typename T, typename ...Args>
void invoke(const std::forward_list<std::unique_ptr<Callback>> &list, Args&& ...args) {
    for (const std::unique_ptr<Callback> &callback : list) {
        assert(callback);
        if (callback->is<T>()) {
            callback->get<T>()(::std::forward<Args>(args)...);
        }
    }
}

BaseRequest::BaseRequest(const std::string &path_) : thread_id(uv_thread_self()), path(path_) {
}

// A base request can only be "canceled" by destroying the object. In that case, we'll have to
// notify all cancel callbacks.
BaseRequest::~BaseRequest() {
    assert(thread_id == uv_thread_self());
    invoke<AbortedCallback>(callbacks);
}

void BaseRequest::notify() {
    assert(thread_id == uv_thread_self());

    // We are only supposed to call notify when a result exists.
    assert(response);

    // The parameter exists solely so that any calls to ->remove()
    // are not going to cause deallocation of this object while this call is in progress.
    util::ptr<BaseRequest> retain = self;

    // Swap the lists so that it's safe for callbacks to call ->cancel()
    // on the request object, which would modify the list.
    const std::forward_list<std::unique_ptr<Callback>> list = std::move(callbacks);
    callbacks.clear();
    invoke<CompletedCallback>(list, *response);

    self.reset();
}

Callback *BaseRequest::add(Callback &&callback, const util::ptr<BaseRequest> &request) {
    assert(thread_id == uv_thread_self());
    assert(this == request.get());

    if (response) {
        // We already have a response. Notify right away.
        if (callback.is<CompletedCallback>()) {
            callback.get<CompletedCallback>()(*response);
        } else {
            // We already know that this request was successful. The AbortedCallback will be discarded
            // here since it would never be called.
        }
        return nullptr;
    } else {
        self = request;
        callbacks.push_front(std::unique_ptr<Callback>(new Callback(std::move(callback))));
        return callbacks.front().get();
    }
}

void BaseRequest::remove(Callback *callback) {
    assert(thread_id == uv_thread_self());
    callbacks.remove_if([=](const std::unique_ptr<Callback> &cb) {
        return cb.get() == callback;
    });
    if (callbacks.empty()) {
        self.reset();
    }
}

}
