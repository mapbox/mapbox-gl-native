#include <mbgl/storage/default_file_source_impl.hpp>
#include <mbgl/storage/asset_context_base.hpp>
#include <mbgl/storage/http_context_base.hpp>
#include <mbgl/storage/network_status.hpp>

#include <mbgl/storage/response.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/log.hpp>

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
    : thread(std::make_unique<util::Thread<Impl>>(
          util::ThreadContext{ "FileSource", util::ThreadType::Unknown, util::ThreadPriority::Low },
          cache,
          root)) {
}

DefaultFileSource::~DefaultFileSource() = default;

std::unique_ptr<FileRequest> DefaultFileSource::request(const Resource& resource, Callback callback) {
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

    Resource res { resource.kind, url };
    auto req = std::make_unique<DefaultFileRequest>(res, *this);
    req->workRequest = thread->invokeWithCallback(&Impl::add, callback, res, req.get());
    return std::move(req);
}

void DefaultFileSource::cancel(const Resource& res, FileRequest* req) {
    thread->invoke(&Impl::cancel, res, req);
}

// ----- Impl -----

DefaultFileSource::Impl::Impl(FileCache* cache_, const std::string& root)
    : cache(cache_),
      assetRoot(root.empty() ? platform::assetRoot() : root),
      assetContext(AssetContextBase::createContext()),
      httpContext(HTTPContextBase::createContext()),
      reachability(std::bind(&Impl::networkIsReachableAgain, this)) {
    // Subscribe to network status changes, but make sure that this async handle doesn't keep the
    // loop alive; otherwise our app wouldn't terminate. After all, we only need status change
    // notifications when our app is still running.
    NetworkStatus::Subscribe(&reachability);
    reachability.unref();
}

DefaultFileSource::Impl::~Impl() {
    NetworkStatus::Unsubscribe(&reachability);
}

void DefaultFileSource::Impl::networkIsReachableAgain() {
    for (auto& req : pending) {
        auto& request = *req.second;
        auto& response = request.getResponse();
        if (!request.realRequest && response && response->error && response->error->reason == Response::Error::Reason::Connection) {
            // We need all requests to fail at least once before we are going to start retrying
            // them, and we only immediately restart request that failed due to connection issues.
            startRealRequest(request);
        }
    }
}

void DefaultFileSource::Impl::add(Resource resource, FileRequest* req, Callback callback) {
    auto& request = *pending.emplace(resource,
        std::make_unique<DefaultFileRequestImpl>(resource)).first->second;

    // Trigger a potentially required refresh of this Request
    update(request);

    // Add this request as an observer so that it'll get notified when something about this
    // request changes.
    request.addObserver(req, callback);
}

void DefaultFileSource::Impl::update(DefaultFileRequestImpl& request) {
    if (request.getResponse()) {
        // We've at least obtained a cache value, potentially we also got a final response.
        // The observers have been notified already; send what we have to the new one as well.

        // Before returning the existing response, make sure that it is still fresh, or update the
        // `stale` flag.
        request.checkResponseFreshness();

        if (request.getResponse()->stale && !request.realRequest) {
            // We've returned a stale response; now make sure the requester also gets a fresh
            // response eventually. It's possible that there's already a request in progress.
            // Note that this will also trigger updates to all other existing listeners.
            // Since we already have data, we're going to verify
            startRealRequest(request);
        } else {
            // The response is still fresh (or there's already a request for refreshing the resource
            // in progress), so there's nothing we need to do.
        }
    } else if (!request.cacheRequest && !request.realRequest) {
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

void DefaultFileSource::Impl::startCacheRequest(DefaultFileRequestImpl& request) {
    // Check the cache for existing data so that we can potentially
    // revalidate the information without having to redownload everything.
    request.cacheRequest =
        cache->get(request.resource, [this, &request](std::shared_ptr<Response> response) {
            request.cacheRequest = nullptr;
            if (response) {
                response->stale = response->isExpired();
                request.setResponse(response);
            }

            if (!response || response->stale) {
                // No response or stale cache. Run the real request.
                startRealRequest(request);
            }

            // Notify in all cases; requestors can decide whether they want to use stale responses.
            request.notify();

            reschedule(request);
        });
}

void DefaultFileSource::Impl::startRealRequest(DefaultFileRequestImpl& request) {
    assert(!request.realRequest);

    // Cancel the timer if we have one.
    if (request.timerRequest) {
        request.timerRequest->stop();
    }

    auto callback = [this, &request](std::shared_ptr<const Response> response) {
        request.realRequest = nullptr;

        if (cache) {
            // Store response in database. Make sure we only refresh the expires column if the data
            // didn't change.
            FileCache::Hint hint = FileCache::Hint::Full;
            if (request.getResponse() && response->data == request.getResponse()->data) {
                hint = FileCache::Hint::Refresh;
            }
            cache->put(request.resource, response, hint);
        }

        request.setResponse(response);
        request.notify();
        reschedule(request);
    };

    if (algo::starts_with(request.resource.url, "asset://")) {
        request.realRequest =
            assetContext->createRequest(request.resource, callback, assetRoot);
    } else {
        request.realRequest =
            httpContext->createRequest(request.resource, callback, request.getResponse());
    }
}

void DefaultFileSource::Impl::cancel(Resource resource, FileRequest* req) {
    auto it = pending.find(resource);
    if (it != pending.end()) {
        // If the number of dependent requests of the DefaultFileRequest drops to zero,
        // cancel the request and remove it from the pending list.
        auto& request = *it->second;
        request.removeObserver(req);
        if (!request.hasObservers()) {
            pending.erase(it);
        }
    } else {
        // There is no request for this URL anymore. Likely, the request already completed
        // before we got around to process the cancelation request.
    }
}

void DefaultFileSource::Impl::reschedule(DefaultFileRequestImpl& request) {
    if (request.realRequest) {
        // There's already a request in progress; don't start another one.
        return;
    }

    const Seconds timeout = request.getRetryTimeout();

    if (timeout == Seconds::zero()) {
        update(request);
    } else if (timeout > Seconds::zero()) {
        if (!request.timerRequest) {
            request.timerRequest = std::make_unique<util::Timer>();
        }

        request.timerRequest->start(timeout, Duration::zero(), [this, &request] {
            assert(!request.realRequest);
            startRealRequest(request);
        });
    }
}

// ----- DefaultFileRequest -----

DefaultFileRequestImpl::~DefaultFileRequestImpl() {
    if (realRequest) {
        realRequest->cancel();
        realRequest = nullptr;
    }
    // timerRequest and cacheRequest are automatically canceld upon destruction.
}

void DefaultFileRequestImpl::addObserver(FileRequest* req, Callback callback) {
    observers.emplace(req, callback);

    if (response) {
        // We've got a response, so send the (potentially stale) response to the requester.
        callback(*response);
    }
}

void DefaultFileRequestImpl::removeObserver(FileRequest* req) {
    observers.erase(req);
}

bool DefaultFileRequestImpl::hasObservers() const {
    return !observers.empty();
}

void DefaultFileRequestImpl::notify() {
    if (response) {
        for (auto& req : observers) {
            req.second(*response);
        }
    }
}

void DefaultFileRequestImpl::setResponse(const std::shared_ptr<const Response>& response_) {
    response = response_;

    if (response->error) {
        failedRequests++;
    } else {
        // Reset the number of subsequent failed requests after we got a successful one.
        failedRequests = 0;
    }
}

const std::shared_ptr<const Response>& DefaultFileRequestImpl::getResponse() const {
    return response;
}

Seconds DefaultFileRequestImpl::getRetryTimeout() const {
    Seconds timeout = Seconds::zero();

    if (!response) {
        // If we don't have a response, we should retry immediately.
        return timeout;
    }

    // A value < 0 means that we should not retry.
    timeout = Seconds(-1);

    if (response->error) {
        assert(failedRequests > 0);
        switch (response->error->reason) {
        case Response::Error::Reason::Server: {
            // Retry immediately, unless we have a certain number of attempts already
            const int graceRetries = 3;
            if (failedRequests <= graceRetries) {
                timeout = Seconds(1);
            } else {
                timeout = Seconds(1 << std::min(failedRequests - graceRetries, 31));
            }
        } break;
        case Response::Error::Reason::Connection: {
            // Exponential backoff
            timeout = Seconds(1 << std::min(failedRequests - 1, 31));
        } break;
        default:
            // Do not retry due to error.
            break;
        }
    }

    // Check to see if this response expires earlier than a potential error retry.
    if (response->expires > Seconds::zero()) {
        const Seconds secsToExpire = response->expires - toSeconds(SystemClock::now());
        // Only update the timeout if we don't have one yet, and only if the new timeout is shorter
        // than the previous one.
        timeout = timeout < Seconds::zero() ? secsToExpire: std::min(timeout, std::max(Seconds::zero(), secsToExpire));
    }

    return timeout;
}

void DefaultFileRequestImpl::checkResponseFreshness() {
    if (response && !response->stale && response->isExpired()) {
        // Create a new Response object with `stale = true`, but the same data, and
        // replace the current request object we have.
        // We're not immediately swapping the member variable because it's declared as `const`, and
        // we first have to update the `stale` flag.
        auto staleResponse = std::make_shared<Response>(*response);
        staleResponse->stale = true;
        response = staleResponse;
    }
}

} // namespace mbgl
