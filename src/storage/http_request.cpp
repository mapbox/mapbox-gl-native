#include <mbgl/storage/http_request.hpp>
#include <mbgl/storage/sqlite_store.hpp>
#include <mbgl/storage/http_request_baton.hpp>
#include <mbgl/platform/log.hpp>

#include <uv.h>

#include <cassert>
#include <chrono>

namespace mbgl {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"

struct CacheRequestBaton {
    HTTPRequest *request = nullptr;
    std::string path;
    util::ptr<SQLiteStore> store;
};

HTTPRequest::HTTPRequest(ResourceType type_, const std::string &path_, uv_loop_t *loop_, util::ptr<SQLiteStore> store_)
    : BaseRequest(path_), thread_id(uv_thread_self()), loop(loop_), store(store_), type(type_) {
    Log::Error(Event::HttpRequest, "HTTPRequest %s", path.c_str());

    if (store) {
        startCacheRequest();
    } else {
        startHTTPRequest(nullptr);
    }
}

void HTTPRequest::startCacheRequest() {
    assert(uv_thread_self() == thread_id);

    cache_baton = new CacheRequestBaton;
    cache_baton->request = this;
    cache_baton->path = path;
    cache_baton->store = store;
    store->get(path, [](std::unique_ptr<Response> &&response, void *ptr) {
        // Wrap in a unique_ptr, so it'll always get auto-destructed.
        std::unique_ptr<CacheRequestBaton> baton((CacheRequestBaton *)ptr);
        if (baton->request) {
            baton->request->cache_baton = nullptr;
            baton->request->handleCacheResponse(std::move(response));
        }
    }, cache_baton);
}

void HTTPRequest::handleCacheResponse(std::unique_ptr<Response> &&res) {
    assert(uv_thread_self() == thread_id);

    if (res) {
        // This entry was stored in the cache. Now determine if we need to revalidate.
        const int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                                std::chrono::system_clock::now().time_since_epoch()).count();
        if (res->expires > now) {
            response = std::move(res);
            notify();
            // Note: after calling notify(), the request object may cease to exist.
            // This HTTPRequest is completed.
            return;
        } else {
            // TODO: notify with preliminary results.
        }
    }

    startHTTPRequest(std::move(res));
}

void HTTPRequest::startHTTPRequest(std::unique_ptr<Response> &&res) {
    assert(uv_thread_self() == thread_id);
    assert(!http_baton);

    http_baton = std::make_shared<HTTPRequestBaton>(path);
    http_baton->request = this;
    http_baton->async = new uv_async_t;
    http_baton->response = std::move(res);
    http_baton->async->data = new util::ptr<HTTPRequestBaton>(http_baton);

#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
    uv_async_init(loop, http_baton->async, [](uv_async_t *async, int) {
#else
    uv_async_init(loop, http_baton->async, [](uv_async_t *async) {
#endif
        util::ptr<HTTPRequestBaton> &baton = *(util::ptr<HTTPRequestBaton> *)async->data;

        if (baton->request) {
            HTTPRequest *request = baton->request;
            request->http_baton.reset();
            baton->request = nullptr;
            request->handleHTTPResponse(baton->type, std::move(baton->response));
        }

        delete (util::ptr<HTTPRequestBaton> *)async->data;
        uv_close((uv_handle_t *)async, [](uv_handle_t *handle) {
            uv_async_t *async_handle = (uv_async_t *)handle;
            delete async_handle;
        });
    });
    attempts++;
    HTTPRequestBaton::start(http_baton);
}



void HTTPRequest::handleHTTPResponse(HTTPResponseType responseType, std::unique_ptr<Response> &&res) {
    assert(uv_thread_self() == thread_id);
    assert(!http_baton);
    assert(!response);

    switch (responseType) {
        // This error was caused by a temporary error and it is likely that it will be resolved
        // immediately. We are going to try again right away. This is like the TemporaryError,
        // except that we will not perform exponential back-off.
        case HTTPResponseType::SingularError:
            if (attempts >= 4) {
                // Report as error after 4 attempts.
                response = std::move(res);
                notify();
            } else if (attempts >= 2) {
                // Switch to the back-off algorithm after the second failure.
                retryHTTPRequest(std::move(res), (1 << attempts) * 1000);
                return;
            } else {
                startHTTPRequest(std::move(res));
            }
            break;

        // This error might be resolved by waiting some time (e.g. server issues).
        // We are going to do an exponential back-off and will try again in a few seconds.
        case HTTPResponseType::TemporaryError:
            if (attempts >= 4) {
                // Report error back after it failed completely.
                response = std::move(res);
                notify();
            } else {
                retryHTTPRequest(std::move(res), (1 << attempts) * 1000);
            }
            break;

        // This error might be resolved once the network reachability status changes.
        // We are going to watch the network status for changes and will retry as soon as the
        // operating system notifies us of a network status change.
        case HTTPResponseType::ConnectionError:

            if (attempts >= 4) {
                // Report error back after it failed completely.
                response = std::move(res);
                notify();
            } else {
                // By default, we will retry every 60 seconds.
                retryHTTPRequest(std::move(res), 60000);
            }
            break;

        // The request was canceled programatically.
        case HTTPResponseType::Canceled:
            response.reset();
            notify();
            break;

        // This error probably won't be resolved by retrying anytime soon. We are giving up.
        case HTTPResponseType::PermanentError:
            response = std::move(res);
            notify();
            break;

        // The request returned data successfully. We retrieved and decoded the data successfully.
        case HTTPResponseType::Successful:
            if (store) {
                store->put(path, type, *res);
            }
            response = std::move(res);
            notify();
            break;

        // The request confirmed that the data wasn't changed. We already have the data.
        case HTTPResponseType::NotModified:
            if (store) {
                store->updateExpiration(path, res->expires);
            }
            response = std::move(res);
            notify();
            break;

        default:
            assert(!"Response wasn't set");
            break;
    }
}

using RetryBaton = std::pair<HTTPRequest *, std::unique_ptr<Response>>;

void HTTPRequest::retryHTTPRequest(std::unique_ptr<Response> &&res, uint64_t timeout) {
    assert(uv_thread_self() == thread_id);
    assert(!backoff_timer);
    backoff_timer = new uv_timer_t();
    uv_timer_init(loop, backoff_timer);
    backoff_timer->data = new RetryBaton(this, std::move(res));

#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
    uv_timer_start(backoff_timer, [](uv_timer_t *timer, int) {
#else
    uv_timer_start(backoff_timer, [](uv_timer_t *timer) {
#endif
        std::unique_ptr<RetryBaton> pair { static_cast<RetryBaton *>(timer->data) };
        pair->first->startHTTPRequest(std::move(pair->second));
        pair->first->backoff_timer = nullptr;
        uv_timer_stop(timer);
        uv_close((uv_handle_t *)timer, [](uv_handle_t *handle) { delete (uv_timer_t *)handle; });
    }, timeout, 0);
}

void HTTPRequest::removeHTTPBaton() {
    assert(uv_thread_self() == thread_id);
    if (http_baton) {
        http_baton->request = nullptr;
        HTTPRequestBaton::stop(http_baton);
        http_baton.reset();
    }
}

void HTTPRequest::removeCacheBaton() {
    assert(uv_thread_self() == thread_id);
    if (cache_baton) {
        // Make sre that this object doesn't accidentally get accessed when it is destructed before
        // the callback returned. They are being run in the same thread, so just setting it to
        // null is sufficient.
        // Note: We don't manually delete the CacheRequestBaton since it'll be deleted by the
        // callback.
        cache_baton->request = nullptr;
        cache_baton = nullptr;
    }
}

void HTTPRequest::removeBackoffTimer() {
    assert(uv_thread_self() == thread_id);
    if (backoff_timer) {
        delete static_cast<RetryBaton *>(backoff_timer->data);
        uv_timer_stop(backoff_timer);
        uv_close((uv_handle_t *)backoff_timer, [](uv_handle_t *handle) { delete (uv_timer_t *)handle; });
        backoff_timer = nullptr;
    }
}

void HTTPRequest::retryImmediately() {
    assert(uv_thread_self() == thread_id);
    if (!cache_baton && !http_baton) {
        if (backoff_timer) {
            // Retry immediately.
            uv_timer_stop(backoff_timer);
            std::unique_ptr<RetryBaton> pair { static_cast<RetryBaton *>(backoff_timer->data) };
            assert(pair->first == this);
            startHTTPRequest(std::move(pair->second));
            uv_close((uv_handle_t *)backoff_timer, [](uv_handle_t *handle) { delete (uv_timer_t *)handle; });
            backoff_timer = nullptr;
        } else {
            assert(!"We should always have a backoff_timer when there are no batons");
        }
    }
}

void HTTPRequest::cancel() {
    assert(uv_thread_self() == thread_id);
    removeCacheBaton();
    removeHTTPBaton();
    removeBackoffTimer();
    notify();
}


HTTPRequest::~HTTPRequest() {
    assert(uv_thread_self() == thread_id);
    cancel();
}

#pragma clang diagnostic pop

}
