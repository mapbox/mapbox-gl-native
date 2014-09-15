#include <mbgl/storage/http_request.hpp>
#include <mbgl/storage/request.hpp>
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
    uv_loop_t *loop;
};

HTTPRequest::HTTPRequest(ResourceType type_, const std::string &path, uv_loop_t *loop, std::shared_ptr<SQLiteStore> store_)
    : BaseRequest(), thread_id(uv_thread_self()), store(store_), type(type_) {
    cache_baton = new CacheRequestBaton;
    cache_baton->request = this;
    cache_baton->path = path;
    cache_baton->loop = loop;
    store->get(path, [](std::unique_ptr<Response> &&response, void *ptr) {
        // Wrap in a unique_ptr, so it'll always get auto-destructed.
        std::unique_ptr<CacheRequestBaton> baton((CacheRequestBaton *)ptr);
        if (baton->request) {
            assert(uv_thread_self() == baton->request->thread_id);
            baton->request->loadedCacheEntry(std::move(response));
            if (baton->request) {
                // If we called notify(), the request object may already have ceased to exist.
                baton->request->cache_baton = nullptr;
            }
        }
    }, cache_baton);
}

void HTTPRequest::loadedCacheEntry(std::unique_ptr<Response> &&response) {
    if (response) {
        // This entry was stored in the cache. Now determine if we need to revalidate.
        const int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                                std::chrono::system_clock::now().time_since_epoch()).count();
        if (response->expires > now) {
            if (cache_baton->request) {
                cache_baton->request->response = std::move(response);
                cache_baton->request->notify();
                // Note: after calling notify(), the request object may cease to exist.
            }
            // This HTTPRequest is completed.
            return;
        } else {
            // TODO: notify with preliminary results.
        }
    }

    // Either, the cache entry is expired, or there is no cached version. In both cases,
    // we need to do a new request. The requesting code will issue a conditional request in case
    // we already have a modified date on the response object.
    http_baton = new HTTPRequestBaton;
    http_baton->request = this;
    http_baton->path = std::move(cache_baton->path);
    http_baton->async = new uv_async_t;
    http_baton->response = std::move(response);
    http_baton->async->data = http_baton;
    uv_async_init(cache_baton->loop, http_baton->async, [](uv_async_t *async) {
        std::unique_ptr<HTTPRequestBaton> baton((HTTPRequestBaton *)async->data);
        if (baton->request) {
            assert(uv_thread_self() == baton->request->thread_id);
            baton->request->response = std::move(baton->response);
            if (baton->not_modified) {
                baton->request->store->updateExpiration(baton->path, baton->request->response->expires);
            } else {
                baton->request->store->put(baton->path, baton->request->type, *baton->request->response);
            }
            baton->request->notify();
            // Note: after calling notify(), the baton object may cease to exist.
        }

        uv_close((uv_handle_t *)async, [](uv_handle_t *handle) {
            delete (uv_async_t *)handle;
        });
    });
    http_baton->start();
}

HTTPRequest::~HTTPRequest() {
    assert(uv_thread_self() == thread_id);

    if (cache_baton) {
        // Make sure that this object doesn't accidentally get accessed when it is destructed before
        // the callback returned. They are being run in the same thread, so just setting it to
        // null is sufficient.
        // Note: We don't manually delete the CacheRequestBaton since it'll be deleted by the
        // callback.
        cache_baton->request = nullptr;
    }

    if (http_baton) {
        http_baton->request = nullptr;
        http_baton->cancel();
    }
}

#pragma clang diagnostic pop

}
