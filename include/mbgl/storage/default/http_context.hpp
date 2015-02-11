#ifndef MBGL_STORAGE_DEFAULT_HTTP_CONTEXT
#define MBGL_STORAGE_DEFAULT_HTTP_CONTEXT

#include "thread_context.hpp"
#include <mbgl/storage/network_status.hpp>

#include <set>

namespace mbgl {

class HTTPRequest;

// This is a template class that provides a per-thread Context object. It can be used by HTTP
// implementations to store global state. It also implements the NetworkStatus mechanism and
// triggers immediate retries on all requests waiting for network status changes.

template <typename Context>
class HTTPContext : public ThreadContext<Context> {
public:
    HTTPContext(uv_loop_t *loop);
    ~HTTPContext();

    void addRequest(HTTPRequest *request);
    void removeRequest(HTTPRequest *baton);

public:
    // Will be fired when the network status becomes reachable.
    uv_async_t *reachability = nullptr;

    // A list of all pending HTTPRequestImpls that we need to notify when the network status
    // changes.
    std::set<HTTPRequest *> requests;
};

template <typename Context>
HTTPContext<Context>::HTTPContext(uv_loop_t *loop_)
    : ThreadContext<Context>(loop_) {
    reachability = new uv_async_t;
    reachability->data = this;
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
    uv_async_init(loop_, reachability, [](uv_async_t *async, int) {
#else
    uv_async_init(loop_, reachability, [](uv_async_t *async) {
#endif
        for (auto request : reinterpret_cast<Context *>(async->data)->requests) {
            request->retryImmediately();
        }
    });
    // Allow the loop to quit even though this handle is still active.
    uv_unref(reinterpret_cast<uv_handle_t *>(reachability));
    NetworkStatus::Subscribe(reachability);
}

template <typename Context>
HTTPContext<Context>::~HTTPContext() {
    MBGL_VERIFY_THREAD(HTTPContext<Context>::tid);

    assert(requests.empty());

    NetworkStatus::Unsubscribe(reachability);
    uv::close(reachability);
}

template <typename Context>
void HTTPContext<Context>::addRequest(HTTPRequest *request) {
    requests.insert(request);
}

template <typename Context>
void HTTPContext<Context>::removeRequest(HTTPRequest *request) {
    requests.erase(request);
}

}

#endif
