#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/http_context_base.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/storage/sqlite_cache.hpp>

#include <mbgl/storage/response.hpp>
#include <mbgl/platform/log.hpp>

#include <mbgl/util/thread.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/timer.hpp>

#include <algorithm>
#include <cassert>
#include <set>
#include <unordered_map>

namespace mbgl {

class OnlineFileRequest : public FileRequest {
public:
    OnlineFileRequest(OnlineFileSource& fileSource_)
        : fileSource(fileSource_) {
    }

    ~OnlineFileRequest() {
        fileSource.cancel(this);
    }

    OnlineFileSource& fileSource;
    std::unique_ptr<WorkRequest> workRequest;
};

class OnlineFileRequestImpl : public util::noncopyable {
public:
    using Callback = std::function<void (Response)>;

    OnlineFileRequestImpl(const Resource&, Callback, OnlineFileSource::Impl&);
    ~OnlineFileRequestImpl();

    void networkIsReachableAgain(OnlineFileSource::Impl&);

private:
    void scheduleCacheRequest(OnlineFileSource::Impl&);
    void scheduleRealRequest(OnlineFileSource::Impl&, bool forceImmediate = false);

    Resource resource;
    std::unique_ptr<WorkRequest> cacheRequest;
    HTTPRequestBase* realRequest = nullptr;
    util::Timer realRequestTimer;
    Callback callback;

    // Counts the number of subsequent failed requests. We're using this value for exponential
    // backoff when retrying requests.
    uint32_t failedRequests = 0;
    Response::Error::Reason failedRequestReason = Response::Error::Reason::Success;
};

class OnlineFileSource::Impl {
public:
    using Callback = std::function<void (Response)>;

    Impl(SQLiteCache*);
    ~Impl();

    void networkIsReachableAgain();

    void add(Resource, FileRequest*, Callback);
    void cancel(FileRequest*);

private:
    friend OnlineFileRequestImpl;

    std::unordered_map<FileRequest*, std::unique_ptr<OnlineFileRequestImpl>> pending;
    SQLiteCache* const cache;
    const std::unique_ptr<HTTPContextBase> httpContext;
    util::AsyncTask reachability;
};

OnlineFileSource::OnlineFileSource(SQLiteCache* cache)
    : thread(std::make_unique<util::Thread<Impl>>(
          util::ThreadContext{ "OnlineFileSource", util::ThreadType::Unknown, util::ThreadPriority::Low },
          cache)) {
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
    auto req = std::make_unique<OnlineFileRequest>(*this);
    req->workRequest = thread->invokeWithCallback(&Impl::add, callback, res, req.get());
    return std::move(req);
}

void OnlineFileSource::cancel(FileRequest* req) {
    thread->invoke(&Impl::cancel, req);
}

// ----- Impl -----

OnlineFileSource::Impl::Impl(SQLiteCache* cache_)
    : cache(cache_),
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
    pending[req] = std::make_unique<OnlineFileRequestImpl>(resource, callback, *this);
}

void OnlineFileSource::Impl::cancel(FileRequest* req) {
    pending.erase(req);
}

// ----- OnlineFileRequest -----

OnlineFileRequestImpl::OnlineFileRequestImpl(const Resource& resource_, Callback callback_, OnlineFileSource::Impl& impl)
    : resource(resource_),
      callback(callback_) {
    if (impl.cache) {
        scheduleCacheRequest(impl);
    } else {
        scheduleRealRequest(impl, true);
    }
}

OnlineFileRequestImpl::~OnlineFileRequestImpl() {
    if (realRequest) {
        realRequest->cancel();
        realRequest = nullptr;
    }
    // realRequestTimer and cacheRequest are automatically canceled upon destruction.
}

void OnlineFileRequestImpl::scheduleCacheRequest(OnlineFileSource::Impl& impl) {
    // Check the cache for existing data so that we can potentially
    // revalidate the information without having to redownload everything.
    cacheRequest = impl.cache->get(resource, [this, &impl](std::shared_ptr<Response> response) {
        cacheRequest = nullptr;

        if (response) {
            resource.priorModified = response->modified;
            resource.priorExpires = response->expires;
            resource.priorEtag = response->etag;
            callback(*response);
        }

        // Force immediate revalidation if we don't have a cached response, or the cached
        // response does not have an expiration time. Otherwise revalidation will happen in
        // the normal scheduling flow.
        scheduleRealRequest(impl, !response || !response->expires);
    });
}

static Duration errorRetryTimeout(Response::Error::Reason failedRequestReason, uint32_t failedRequests) {
    if (failedRequestReason == Response::Error::Reason::Server) {
        // Retry after one second three times, then start exponential backoff.
        return Seconds(failedRequests <= 3 ? 1 : 1 << std::min(failedRequests - 3, 31u));
    } else if (failedRequestReason == Response::Error::Reason::Connection) {
        // Immediate exponential backoff.
        assert(failedRequests > 0);
        return Seconds(1 << std::min(failedRequests - 1, 31u));
    } else {
        // No error, or not an error that triggers retries.
        return Duration::max();
    }
}

static Duration expirationTimeout(optional<SystemTimePoint> expires) {
    if (expires) {
        return std::max(SystemDuration::zero(), *expires - SystemClock::now());
    } else {
        return Duration::max();
    }
}

void OnlineFileRequestImpl::scheduleRealRequest(OnlineFileSource::Impl& impl, bool forceImmediate) {
    if (realRequest) {
        // There's already a request in progress; don't start another one.
        return;
    }

    // If we're not being asked for a forced refresh, calculate a timeout that depends on how many
    // consecutive errors we've encountered, and on the expiration time, if present.
    Duration timeout = forceImmediate
        ? Duration::zero()
        : std::min(errorRetryTimeout(failedRequestReason, failedRequests),
                   expirationTimeout(resource.priorExpires));

    if (timeout == Duration::max()) {
        return;
    }

    realRequestTimer.start(timeout, Duration::zero(), [this, &impl] {
        assert(!realRequest);
        realRequest = impl.httpContext->createRequest(resource, [this, &impl](Response response) {
            realRequest = nullptr;

            // If we didn't get various caching headers in the response, continue using the
            // previous values. Otherwise, update the previous values to the new values.

            if (!response.modified) {
                response.modified = resource.priorModified;
            } else {
                resource.priorModified = response.modified;
            }

            if (!response.expires) {
                response.expires = resource.priorExpires;
            } else {
                resource.priorExpires = response.expires;
            }

            if (!response.etag) {
                response.etag = resource.priorEtag;
            } else {
                resource.priorEtag = response.etag;
            }

            if (impl.cache) {
                impl.cache->put(resource, response);
            }

            if (response.error) {
                failedRequests++;
                failedRequestReason = response.error->reason;
            } else {
                failedRequests = 0;
                failedRequestReason = Response::Error::Reason::Success;
            }

            callback(response);
            scheduleRealRequest(impl);
        });
    });
}

void OnlineFileRequestImpl::networkIsReachableAgain(OnlineFileSource::Impl& impl) {
    // We need all requests to fail at least once before we are going to start retrying
    // them, and we only immediately restart request that failed due to connection issues.
    if (failedRequestReason == Response::Error::Reason::Connection) {
        scheduleRealRequest(impl, true);
    }
}

} // namespace mbgl
