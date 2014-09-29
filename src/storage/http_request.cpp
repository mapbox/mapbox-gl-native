#include <mbgl/storage/http_request.hpp>
#include <mbgl/storage/sqlite_store.hpp>
#include <mbgl/storage/http_request_baton.hpp>

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

HTTPRequest::HTTPRequest(ResourceType type_, const std::string &path, uv_loop_t *loop_, util::ptr<SQLiteStore> store_)
    : BaseRequest(path), thread_id(uv_thread_self()), loop(loop_), store(store_), type(type_) {
    startCacheRequest();
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

    uv_async_init(loop, http_baton->async, [](uv_async_t *async) {
        util::ptr<HTTPRequestBaton> &http_baton = *(util::ptr<HTTPRequestBaton> *)async->data;

        if (http_baton->request) {
            HTTPRequest *request = http_baton->request;
            request->http_baton.reset();
            http_baton->request = nullptr;
            request->handleHTTPResponse(http_baton->type, std::move(http_baton->response));
        }

        delete (util::ptr<HTTPRequestBaton> *)async->data;
        uv_close((uv_handle_t *)async, [](uv_handle_t *handle) {
            uv_async_t *async = (uv_async_t *)handle;
            delete async;
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
        case HTTPResponseType::TemporaryError:
        case HTTPResponseType::SingularError:
        case HTTPResponseType::ConnectionError:
            fprintf(stderr, "error: %s\n", res->message.c_str());
            // TODO: retry
            break;

        case HTTPResponseType::Canceled:
            response.reset();
            notify();
            break;

        case HTTPResponseType::PermanentError:
            response = std::move(res);
            notify();
            break;

        case HTTPResponseType::Successful:
            store->put(path, type, *res);
            response = std::move(res);
            notify();
            break;

        case HTTPResponseType::NotModified:
            store->updateExpiration(path, res->expires);
            response = std::move(res);
            notify();
            break;

        default:
            assert(!"Response wasn't set");
            break;
    }
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

void HTTPRequest::cancel() {
    assert(uv_thread_self() == thread_id);
    removeCacheBaton();
    removeHTTPBaton();
    notify();
}


HTTPRequest::~HTTPRequest() {
    assert(uv_thread_self() == thread_id);
    cancel();
}

#pragma clang diagnostic pop

}
