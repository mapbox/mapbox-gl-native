#include <mbgl/storage/request.hpp>
#include <mbgl/storage/base_request.hpp>

#include <uv.h>

#include <cassert>

namespace mbgl {

Request::Request(const std::shared_ptr<BaseRequest> &base_)
    : thread_id(uv_thread_self()), base(base_) {
}

Request::~Request() {
    assert(thread_id == uv_thread_self());
}

void Request::onload(Callback cb) {
    assert(thread_id == uv_thread_self());
    if (base) {
        Callback *callback = base->add(std::move(cb), base);
        if (callback) {
            callbacks.push_front(callback);
        }
    }
}

void Request::cancel() {
    assert(thread_id == uv_thread_self());
    if (base) {
        for (Callback *callback : callbacks) {
            base->remove(callback);
        }
        base.reset();
    }
    callbacks.clear();
}

}
