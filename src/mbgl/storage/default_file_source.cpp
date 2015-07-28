#include <mbgl/storage/default_file_source_impl.hpp>
#include <mbgl/storage/request.hpp>
#include <mbgl/storage/asset_context.hpp>
#include <mbgl/storage/http_context_base.hpp>

#include <mbgl/storage/response.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/log.hpp>

#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/mapbox.hpp>

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
    : thread(std::make_unique<util::Thread<Impl>>(util::ThreadContext{"FileSource", util::ThreadType::Unknown, util::ThreadPriority::Low}, cache, root)) {
}

DefaultFileSource::~DefaultFileSource() {
    MBGL_VERIFY_THREAD(tid);
}

Request* DefaultFileSource::request(const Resource& resource,
                                    uv_loop_t* l,
                                    Callback callback) {
    assert(l);

    std::string url;

    switch (resource.kind) {
    case Resource::Kind::Style:
        url = mbgl::util::mapbox::normalizeStyleURL(resource.url, accessToken);
        break;

    case Resource::Kind::Source:
        url = util::mapbox::normalizeSourceURL(resource.url, accessToken);
        break;

    case Resource::Kind::Glyphs:
        url = util::mapbox::normalizeGlyphsURL(resource.url, accessToken);
        break;

    default:
        url = resource.url;
    }

    auto req = new Request({ resource.kind, url }, l, std::move(callback));
    thread->invoke(&Impl::add, req);
    return req;
}

void DefaultFileSource::cancel(Request *req) {
    req->cancel();
    thread->invoke(&Impl::cancel, req);
}

// ----- Impl -----

DefaultFileSource::Impl::Impl(FileCache* cache_, const std::string& root)
    : loop(util::RunLoop::getLoop()),
      cache(cache_),
      assetRoot(root.empty() ? platform::assetRoot() : root),
      assetContext(AssetContext::createContext(loop)),
      httpContext(HTTPContextBase::createContext(loop)) {
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

    request = &pending.emplace(resource, resource).first->second;
    request->observers.insert(req);

    if (cache) {
        startCacheRequest(request);
    } else {
        startRealRequest(request);
    }
}

void DefaultFileSource::Impl::startCacheRequest(DefaultFileRequest* request) {
    // Check the cache for existing data so that we can potentially
    // revalidate the information without having to redownload everything.
    request->cacheRequest = cache->get(request->resource, [this, request](std::unique_ptr<Response> response) {
        auto expired = [&response] {
            const int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                                    SystemClock::now().time_since_epoch()).count();
            return response->expires <= now;
        };

        if (!response || expired()) {
            // No response or stale cache. Run the real request.
            startRealRequest(request, std::move(response));
        } else {
            // The response is fresh. We're good to notify the caller.
            notify(request, std::move(response), FileCache::Hint::No);
        }
    });
}

void DefaultFileSource::Impl::startRealRequest(DefaultFileRequest* request, std::shared_ptr<const Response> response) {
    auto callback = [request, this] (std::shared_ptr<const Response> res, FileCache::Hint hint) {
        notify(request, res, hint);
    };

    if (algo::starts_with(request->resource.url, "asset://")) {
        request->realRequest = assetContext->createRequest(request->resource, callback, loop, assetRoot);
    } else {
        request->realRequest = httpContext->createRequest(request->resource, callback, loop, response);
    }
}

void DefaultFileSource::Impl::cancel(Request* req) {
    DefaultFileRequest* request = find(req->resource);

    if (request) {
        // If the number of dependent requests of the DefaultFileRequest drops to zero,
        // cancel the request and remove it from the pending list.
        request->observers.erase(req);
        if (request->observers.empty()) {
            if (request->cacheRequest) {
                request->cacheRequest.reset();
            }
            if (request->realRequest) {
                request->realRequest->cancel();
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
