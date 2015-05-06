#include <mbgl/storage/default_file_source_impl.hpp>
#include <mbgl/storage/request.hpp>
#include <mbgl/storage/asset_context.hpp>
#include <mbgl/storage/http_context.hpp>

#include <mbgl/storage/response.hpp>
#include <mbgl/platform/platform.hpp>

#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/platform/log.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/algorithm/string.hpp>
#pragma GCC diagnostic pop

#include <algorithm>
#include <cassert>


namespace algo = boost::algorithm;

namespace mbgl {

DefaultFileSource::DefaultFileSource(FileCache* cache, const std::string& root)
    : thread(util::make_unique<util::Thread<Impl>>("FileSource", util::ThreadPriority::Low, cache, root)) {
}

DefaultFileSource::~DefaultFileSource() {
    MBGL_VERIFY_THREAD(tid);
}

Request* DefaultFileSource::request(const Resource& resource,
                                    uv_loop_t* l,
                                    Callback callback) {
    auto req = new Request(resource, l, std::move(callback));

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue.
    thread->invoke(&Impl::add, req);

    return req;
}

void DefaultFileSource::request(const Resource& resource, Callback callback) {
    request(resource, nullptr, std::move(callback));
}

void DefaultFileSource::cancel(Request *req) {
    req->cancel();

    // This function can be called from any thread. Make sure we're executing the actual call in the
    // file source loop by sending it over the queue.
    thread->invoke(&Impl::cancel, req);
}

// ----- Impl -----

DefaultFileSource::Impl::Impl(uv_loop_t* loop_, FileCache* cache_, const std::string& root)
    : loop(loop_),
      cache(cache_),
      assetRoot(root.empty() ? platform::assetRoot() : root),
      assetContext(AssetContext::createContext(loop_)),
      httpContext(HTTPContext::createContext(loop_)) {
}

DefaultFileRequest* DefaultFileSource::Impl::find(const Resource& resource) {
    const auto it = pending.find(resource);
    if (it != pending.end()) {
        return &it->second;
    }
    return nullptr;
}

void DefaultFileSource::Impl::add(Request* req) {
    const Resource& resource = req->resource;
    DefaultFileRequest* request = find(resource);

    if (request) {
        request->observers.insert(req);
        return;
    }

    request = &pending.emplace(resource, DefaultFileRequest(resource)).first->second;
    request->observers.insert(req);

    if (cache) {
        startCacheRequest(resource);
    } else {
        startRealRequest(resource);
    }
}

void DefaultFileSource::Impl::startCacheRequest(const Resource& resource) {
    // Check the cache for existing data so that we can potentially
    // revalidate the information without having to redownload everything.
    cache->get(resource, [this, resource](std::unique_ptr<Response> response) {
        DefaultFileRequest* request = find(resource);

        if (!request) {
            // There is no request for this URL anymore. Likely, the request was canceled
            // before we got around to process the cache result.
            return;
        }

        auto expired = [&response] {
            const int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                                    SystemClock::now().time_since_epoch()).count();
            return response->expires <= now;
        };

        if (!response || expired()) {
            // No response or stale cache. Run the real request.
            startRealRequest(resource, std::move(response));
        } else {
            // The response is fresh. We're good to notify the caller.
            notify(request, std::move(response), FileCache::Hint::No);
        }
    });
}

void DefaultFileSource::Impl::startRealRequest(const Resource& resource, std::shared_ptr<const Response> response) {
    DefaultFileRequest* request = find(resource);

    auto callback = [request, this] (std::shared_ptr<const Response> res, FileCache::Hint hint) {
        notify(request, res, hint);
    };

    if (algo::starts_with(resource.url, "asset://")) {
        request->request = assetContext->createRequest(resource, callback, loop, assetRoot);
    } else {
        request->request = httpContext->createRequest(resource, callback, loop, response);
    }
}

void DefaultFileSource::Impl::cancel(Request* req) {
    DefaultFileRequest* request = find(req->resource);

    if (request) {
        // If the number of dependent requests of the DefaultFileRequest drops to zero,
        // cancel the request and remove it from the pending list.
        request->observers.erase(req);
        if (request->observers.empty()) {
            if (request->request) {
                request->request->cancel();
            }
            pending.erase(request->resource);
        }
    } else {
        // There is no request for this URL anymore. Likely, the request already completed
        // before we got around to process the cancelation request.
    }

    // Send a message back to the requesting thread and notify it that this request has been
    // canceled and is now safe to be deleted.
    req->destruct();
}

void DefaultFileSource::Impl::notify(DefaultFileRequest* request, std::shared_ptr<const Response> response, FileCache::Hint hint) {
    // First, remove the request, since it might be destructed at any point now.
    assert(find(request->resource) == request);
    assert(response);

    // Notify all observers.
    for (auto req : request->observers) {
        req->notify(response);
    }

    if (cache) {
        // Store response in database
        cache->put(request->resource, response, hint);
    }

    pending.erase(request->resource);
}

}
