#ifndef MBGL_STORAGE_DEFAULT_HTTP_CONTEXT
#define MBGL_STORAGE_DEFAULT_HTTP_CONTEXT

#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/uv.hpp>

#include <uv.h>
#include <pthread.h>

#include <map>
#include <cassert>
#include <set>

namespace mbgl {

class HTTPRequest;

// This is a template class that provides a per-thread Context object. It can be used by HTTP
// implementations to store global state. It also implements the NetworkStatus mechanism and
// triggers immediate retries on all requests waiting for network status changes.

template <typename Context>
class HTTPContext : private util::noncopyable {
protected:
    MBGL_STORE_THREAD(tid)
    using Map = std::map<uv_loop_t *, std::unique_ptr<Context>>;

public:
    static Context *Get(uv_loop_t *loop);

private:
    static pthread_key_t key;
    static pthread_once_t once;

public:
    HTTPContext(uv_loop_t *loop);
    ~HTTPContext();

    void addRequest(HTTPRequest *request);
    void removeRequest(HTTPRequest *baton);

public:
    uv_loop_t *loop;

    // Will be fired when the network status becomes reachable.
    uv_async_t *reachability = nullptr;

    // A list of all pending HTTPRequestImpls that we need to notify when the network status
    // changes.
    std::set<HTTPRequest *> requests;
};

template <typename Context>
Context *HTTPContext<Context>::Get(uv_loop_t *loop) {
    pthread_once(&once, []() {
        pthread_key_create(&key, [](void *ptr) {
            assert(ptr);
            delete reinterpret_cast<Map *>(ptr);
        });
    });
    auto contexts = reinterpret_cast<Map *>(pthread_getspecific(key));
    if (!contexts) {
        contexts = new Map();
        pthread_setspecific(key, contexts);
    }

    // Now find a HTTPContext that matches the requested loop.
    auto it = contexts->find(loop);
    if (it == contexts->end()) {
        auto result = contexts->emplace(loop, util::make_unique<Context>(loop));
        assert(result.second); // Make sure it was actually inserted.
        return result.first->second.get();
    } else {
        return it->second.get();
    }
}

template <typename Context>
HTTPContext<Context>::HTTPContext(uv_loop_t *loop_) : loop(loop_) {
    reachability = new uv_async_t;
    reachability->data = this;
    uv_async_init(loop, reachability, [](uv_async_t *async, int) {
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
    MBGL_VERIFY_THREAD(tid);

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
