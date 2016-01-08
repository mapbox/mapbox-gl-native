#include <mbgl/storage/online_file_source.hpp>
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
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/timer.hpp>
#include <mbgl/util/url.hpp>

#include <algorithm>
#include <cassert>
#include <set>
#include <unordered_map>

namespace mbgl {

class RequestBase;

class OnlineFileRequest : public FileRequest {
public:
    OnlineFileRequest(const Resource& resource_,
                       OnlineFileSource& fileSource_)
        : resource(resource_),
          fileSource(fileSource_) {
    }

    ~OnlineFileRequest() {
        fileSource.cancel(resource, this);
    }

    Resource resource;
    OnlineFileSource& fileSource;

    std::unique_ptr<WorkRequest> workRequest;
};

class OnlineFileRequestImpl : public util::noncopyable {
public:
    using Callback = std::function<void (Response)>;

    OnlineFileRequestImpl(const Resource&);
    ~OnlineFileRequestImpl();

    // Observer accessors.
    void addObserver(FileRequest*, Callback, OnlineFileSource::Impl&);
    void removeObserver(FileRequest*);
    bool hasObservers() const;

    void networkIsReachableAgain(OnlineFileSource::Impl&);

private:
    void scheduleCacheRequest(OnlineFileSource::Impl&);
    void scheduleRealRequest(OnlineFileSource::Impl&, bool forceImmediate = false);

    const Resource resource;
    std::unique_ptr<WorkRequest> cacheRequest;
    RequestBase* realRequest = nullptr;
    util::Timer realRequestTimer;

    // Stores a set of all observing Request objects.
    std::unordered_map<FileRequest*, Callback> observers;

    // The current response data. We're storing it because we can satisfy requests for the same
    // resource directly by returning this response object. We also need it to create conditional
    // HTTP requests, and to check whether new responses we got changed any data.
    std::shared_ptr<const Response> response;

    // Counts the number of subsequent failed requests. We're using this value for exponential
    // backoff when retrying requests.
    int failedRequests = 0;
};

class OnlineFileSource::Impl {
public:
    using Callback = std::function<void (Response)>;

    Impl(FileCache*, const std::string& = "");
    ~Impl();

    void networkIsReachableAgain();

    void add(Resource, FileRequest*, Callback);
    void cancel(Resource, FileRequest*);

private:
    friend OnlineFileRequestImpl;

    std::unordered_map<Resource, std::unique_ptr<OnlineFileRequestImpl>, Resource::Hash> pending;
    FileCache* const cache;
    const std::string assetRoot;
    const std::unique_ptr<AssetContextBase> assetContext;
    const std::unique_ptr<HTTPContextBase> httpContext;
    util::AsyncTask reachability;
};

OnlineFileSource::OnlineFileSource(FileCache* cache, const std::string& root)
    : thread(std::make_unique<util::Thread<Impl>>(
          util::ThreadContext{ "OnlineFileSource", util::ThreadType::Unknown, util::ThreadPriority::Low },
          cache,
          root)) {
}

OnlineFileSource::~OnlineFileSource() = default;

std::unique_ptr<FileRequest> OnlineFileSource::request(const Resource& resource, Callback callback) {
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
    auto req = std::make_unique<OnlineFileRequest>(res, *this);
    req->workRequest = thread->invokeWithCallback(&Impl::add, callback, res, req.get());
    return std::move(req);
}

void OnlineFileSource::cancel(const Resource& res, FileRequest* req) {
    thread->invoke(&Impl::cancel, res, req);
}

// ----- Impl -----

OnlineFileSource::Impl::Impl(FileCache* cache_, const std::string& root)
    : cache(cache_),
      assetRoot(root.empty() ? platform::assetRoot() : root),
      assetContext(AssetContextBase::createContext()),
      httpContext(HTTPContextBase::createContext()),
      reachability(std::bind(&Impl::networkIsReachableAgain, this)) {
    // Subscribe to network status changes, but make sure that this async handle doesn't keep the
    // loop alive; otherwise our app wouldn't terminate. After all, we only need status change
    // notifications when our app is still running.
    NetworkStatus::Subscribe(&reachability);
}

OnlineFileSource::Impl::~Impl() {
    NetworkStatus::Unsubscribe(&reachability);
}

void OnlineFileSource::Impl::networkIsReachableAgain() {
    for (auto& req : pending) {
        req.second->networkIsReachableAgain(*this);
    }
}

void OnlineFileSource::Impl::add(Resource resource, FileRequest* req, Callback callback) {
    auto& request = *pending.emplace(resource,
        std::make_unique<OnlineFileRequestImpl>(resource)).first->second;

    // Add this request as an observer so that it'll get notified when something about this
    // request changes.
    request.addObserver(req, callback, *this);
}

void OnlineFileSource::Impl::cancel(Resource resource, FileRequest* req) {
    auto it = pending.find(resource);
    if (it != pending.end()) {
        // If the number of dependent requests of the OnlineFileRequest drops to zero,
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

// ----- OnlineFileRequest -----

OnlineFileRequestImpl::OnlineFileRequestImpl(const Resource& resource_)
    : resource(resource_) {
}

OnlineFileRequestImpl::~OnlineFileRequestImpl() {
    if (realRequest) {
        realRequest->cancel();
        realRequest = nullptr;
    }
    // realRequestTimer and cacheRequest are automatically canceled upon destruction.
}

void OnlineFileRequestImpl::addObserver(FileRequest* req, Callback callback, OnlineFileSource::Impl& impl) {
    if (response) {
        // We've at least obtained a cache value, potentially we also got a final response.
        // Before returning the existing response, update the `stale` flag if necessary.
        if (!response->stale && response->isExpired()) {
            // Create a new Response object with `stale = true`, but the same data, and
            // replace the current request object we have.
            // We're not immediately swapping the member variable because it's declared as `const`, and
            // we first have to update the `stale` flag.
            auto staleResponse = std::make_shared<Response>(*response);
            staleResponse->stale = true;
            response = staleResponse;
        }

        callback(*response);

        if (response->stale && !realRequest) {
            // We've returned a stale response; now make sure the requester also gets a fresh
            // response eventually. It's possible that there's already a request in progress.
            // Note that this will also trigger updates to all other existing listeners.
            // Since we already have data, we're going to verify
            scheduleRealRequest(impl);
        } else {
            // The response is still fresh (or there's already a request for refreshing the resource
            // in progress), so there's nothing we need to do.
        }
    } else if (!cacheRequest && !realRequest) {
        // There is no request in progress, and we don't have a response yet. This means we'll have
        // to start the request ourselves.
        if (impl.cache && !util::isAssetURL(resource.url)) {
            scheduleCacheRequest(impl);
        } else {
            scheduleRealRequest(impl);
        }
    } else {
        // There is a request in progress. We just have to wait.
    }

    observers.emplace(req, callback);
}

void OnlineFileRequestImpl::removeObserver(FileRequest* req) {
    observers.erase(req);
}

bool OnlineFileRequestImpl::hasObservers() const {
    return !observers.empty();
}

void OnlineFileRequestImpl::scheduleCacheRequest(OnlineFileSource::Impl& impl) {
    // Check the cache for existing data so that we can potentially
    // revalidate the information without having to redownload everything.
    cacheRequest = impl.cache->get(resource, [this, &impl](std::shared_ptr<Response> response_) {
        cacheRequest = nullptr;

        if (response_) {
            response_->stale = response_->isExpired();
            response = response_;
            for (auto& req : observers) {
                req.second(*response);
            }
        }

        scheduleRealRequest(impl);
    });
}

void OnlineFileRequestImpl::scheduleRealRequest(OnlineFileSource::Impl& impl, bool forceImmediate) {
    if (realRequest) {
        // There's already a request in progress; don't start another one.
        return;
    }

    // If we don't have a response, we should retry immediately.
    // A value < 0 means that we should not retry.
    Seconds timeout = (response && !response->stale) ? Seconds(-1) : Seconds::zero();

    if (response && response->error) {
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
    if (response && response->expires > Seconds::zero()) {
        const Seconds secsToExpire = response->expires - toSeconds(SystemClock::now());
        // Only update the timeout if we don't have one yet, and only if the new timeout is shorter
        // than the previous one.
        timeout = timeout < Seconds::zero() ? secsToExpire: std::min(timeout, std::max(Seconds::zero(), secsToExpire));
    }

    if (forceImmediate) {
        timeout = Seconds::zero();
    }

    if (timeout < Seconds::zero()) {
        return;
    }

    realRequestTimer.start(timeout, Duration::zero(), [this, &impl] {
        assert(!realRequest);

        auto callback = [this, &impl](std::shared_ptr<const Response> response_) {
            realRequest = nullptr;

            // Only update the cache for successful or 404 responses.
            // In particular, we don't want to write a Canceled request, or one that failed due to
            // connection errors to the cache. Server errors are hopefully also temporary, so we're not
            // caching them either.
            if (impl.cache && !util::isAssetURL(resource.url) &&
                (!response_->error || (response_->error->reason == Response::Error::Reason::NotFound))) {
                // Store response in database. Make sure we only refresh the expires column if the data
                // didn't change.
                FileCache::Hint hint = FileCache::Hint::Full;
                if (response && response_->data == response->data) {
                    hint = FileCache::Hint::Refresh;
                }
                impl.cache->put(resource, response_, hint);
            }

            response = response_;

            if (response->error) {
                failedRequests++;
            } else {
                // Reset the number of subsequent failed requests after we got a successful one.
                failedRequests = 0;
            }

            for (auto& req : observers) {
                req.second(*response);
            }

            scheduleRealRequest(impl);
        };

        if (util::isAssetURL(resource.url)) {
            realRequest = impl.assetContext->createRequest(resource.url, callback, impl.assetRoot);
        } else {
            realRequest = impl.httpContext->createRequest(resource.url, callback, response);
        }
    });
}

void OnlineFileRequestImpl::networkIsReachableAgain(OnlineFileSource::Impl& impl) {
    // We need all requests to fail at least once before we are going to start retrying
    // them, and we only immediately restart request that failed due to connection issues.
    if (response && response->error && response->error->reason == Response::Error::Reason::Connection) {
        scheduleRealRequest(impl, true);
    }
}

} // namespace mbgl
