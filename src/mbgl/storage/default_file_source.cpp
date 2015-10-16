#include <mbgl/storage/default_file_source_impl.hpp>
#include <mbgl/storage/request.hpp>
#include <mbgl/storage/asset_context_base.hpp>
#include <mbgl/storage/http_context_base.hpp>

#include <mbgl/storage/response.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/log.hpp>

#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/chrono.hpp>

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

    if (!callback) {
        throw util::MisuseException("FileSource callback can't be empty");
    }

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

    case Resource::Kind::SpriteImage:
    case Resource::Kind::SpriteJSON:
        url = util::mapbox::normalizeSpriteURL(resource.url, accessToken);
        break;

    default:
        url = resource.url;
    }

    auto req = new Request({ resource.kind, url }, l, std::move(callback));
    thread->invoke(&Impl::add, req);
    return req;
}

void DefaultFileSource::cancel(Request *req) {
    assert(req);
    req->cancel();
    thread->invoke(&Impl::cancel, req);
}

// ----- Impl -----

DefaultFileSource::Impl::Impl(FileCache* cache_, const std::string& root)
    : loop(util::RunLoop::getLoop()),
      cache(cache_),
      assetRoot(root.empty() ? platform::assetRoot() : root),
      assetContext(AssetContextBase::createContext(loop)),
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

    if (!request) {
        request = &pending.emplace(resource, resource).first->second;
    }

    // Add this request as an observer so that it'll get notified when something about this
    // request changes.
    request->observers.insert(req);

    update(request);

    if (request->response) {
        // We've got a response, so send the (potentially stale) response to the requester.
        req->notify(request->response);
    }
}

void DefaultFileSource::Impl::update(DefaultFileRequest* request) {
    if (request->response) {
        // We've at least obtained a cache value, potentially we also got a final response.
        // The observers have been notified already; send what we have to the new one as well.

        // Before returning the existing response, make sure that it is still fresh.
        if (!request->response->stale && request->response->isExpired()) {
            // Create a new Response object with `stale = true`, but the same data, and
            // replace the current request object we have.
            auto response = std::make_shared<Response>(*request->response);
            response->stale = true;
            request->response = response;
        }

        if (request->response->stale && !request->realRequest) {
            // We've returned a stale response; now make sure the requester also gets a fresh
            // response eventually. It's possible that there's already a request in progress.
            // Note that this will also trigger updates to all other existing listeners.
            // Since we already have data, we're going to verify
            startRealRequest(request, request->response);
        }
    } else if (!request->cacheRequest && !request->realRequest) {
        // There is no request in progress, and we don't have a response yet. This means we'll have
        // to start the request ourselves.
        if (cache) {
            startCacheRequest(request);
        } else {
            startRealRequest(request);
        }
    } else {
        // There is a request in progress. We just have to wait.
    }
}

void DefaultFileSource::Impl::startCacheRequest(DefaultFileRequest* request) {
    // Check the cache for existing data so that we can potentially
    // revalidate the information without having to redownload everything.
    request->cacheRequest = cache->get(request->resource, [this, request](std::shared_ptr<Response> response) {
        request->cacheRequest = nullptr;
        if (response) {
            response->stale = response->isExpired();
        }

        if (!response || response->stale) {
            // No response or stale cache. Run the real request.
            startRealRequest(request, response);
        }

        if (response) {
            // Notify in all cases; requestors can decide whether they want to use stale responses.
            notify(request, response, FileCache::Hint::No);
        }
    });
}

void DefaultFileSource::Impl::startRealRequest(DefaultFileRequest* request, std::shared_ptr<const Response> response) {
    // Cancel the timer if we have one.
    if (request->timerRequest) {
        request->timerRequest->stop();
    }

    auto callback = [request, this] (std::shared_ptr<const Response> res, FileCache::Hint hint) {
        request->realRequest = nullptr;
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
    request->response = response;
    for (auto req : request->observers) {
        req->notify(response);
    }

    if (cache) {
        // Store response in database
        cache->put(request->resource, response, hint);
    }

    // Set timer for requests that have a known expiry times. Expiry times of 0 are technically
    // expiring immediately, but we can't continually request.
    if (!request->realRequest && response->expires > 0) {
        const int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
                                    SystemClock::now().time_since_epoch()).count();
        const int64_t timeout = response->expires - now;

        if (timeout <= 0) {
            update(request);
        } else {
            if (!request->timerRequest) {
                request->timerRequest = std::make_unique<uv::timer>(util::RunLoop::getLoop());
            }

            // timeout is in seconds, but the timer takes milliseconds.
            request->timerRequest->start(1000 * timeout, 0, [this, request] {
                update(request);
            });
        }
    }
}

}
