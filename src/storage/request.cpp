#include <mbgl/storage/request.hpp>
#include <mbgl/storage/base_request.hpp>

#include <uv.h>

#include <cassert>

namespace mbgl {

Request::Request(const util::ptr<BaseRequest> &base_)
    : thread_id(std::this_thread::get_id()), base(base_) {
}

Request::~Request() {
    assert(thread_id == std::this_thread::get_id());
}

void Request::onload(CompletedCallback cb) {
    assert(thread_id == std::this_thread::get_id());
    if (base) {
        Callback *callback = base->add(std::move(cb), base);
        if (callback) {
            callbacks.push_front(callback);
        }
    }
}

void Request::oncancel(AbortedCallback cb) {
    assert(thread_id == std::this_thread::get_id());
    if (base) {
        Callback *callback = base->add(std::move(cb), base);
        if (callback) {
            callbacks.push_front(callback);
        }
    }
}

void Request::cancel() {
    assert(thread_id == std::this_thread::get_id());
    if (base) {
        for (Callback *callback : callbacks) {
            base->remove(callback);
        }
        base.reset();
    }
    callbacks.clear();
}

}
