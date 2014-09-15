#include <mbgl/storage/base_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/request.hpp>

#include <uv.h>

#include <cassert>

namespace mbgl {

BaseRequest::BaseRequest() : thread_id(uv_thread_self()) {
}

BaseRequest::~BaseRequest() {
    assert(thread_id == uv_thread_self());
}

void BaseRequest::notify() {
    assert(thread_id == uv_thread_self());

    // The parameter exists solely so that any calls to ->remove()
    // are not going to cause deallocation of this object while this call is in progress.
    std::shared_ptr<BaseRequest> retain = self;

    // Swap the lists so that it's safe for callbacks to call ->cancel()
    // on the request object, which would modify the list.
    std::forward_list<std::unique_ptr<Callback>> list;
    list.swap(callbacks);

    // Only notify when we actually have a response. When the response code is 0, the request was
    // canceled and we don't need to notify.
    if (response) {
        // Now that we have our private copy, notify all observers.
        for (std::unique_ptr<Callback> &callback : list) {
            (*callback)(*response);
        }
    }

    self.reset();
}

Callback *BaseRequest::add(Callback &&callback, const std::shared_ptr<BaseRequest> &request) {
    assert(thread_id == uv_thread_self());
    assert(this == request.get());
    if (response) {
        // We already have a response. Notify right away.
        callback(*response);
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
