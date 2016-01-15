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

    const Resource resource;
    std::unique_ptr<WorkRequest> cacheRequest;
    HTTPRequestBase* realRequest = nullptr;
    util::Timer realRequestTimer;
    Callback callback;

    // The current response data. Used to create conditional HTTP requests, and to check whether
    // new responses we got changed any data.
    std::shared_ptr<const Response> response;

    // Counts the number of subsequent failed requests. We're using this value for exponential
    // backoff when retrying requests.
    uint32_t failedRequests = 0;
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
        scheduleRealRequest(impl);
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
    cacheRequest = impl.cache->get(resource, [this, &impl](std::shared_ptr<Response> response_) {
        cacheRequest = nullptr;

        if (response_) {
            response_->stale = response_->isExpired();
            response = response_;
            callback(*response);
        }

        scheduleRealRequest(impl);
    });
}

static Seconds errorRetryTimeout(const Response& response, uint32_t failedRequests) {
    if (response.error && response.error->reason == Response::Error::Reason::Server) {
        // Retry after one second three times, then start exponential backoff.
        return Seconds(failedRequests <= 3 ? 1 : 1 << std::min(failedRequests - 3, 31u));
    } else if (response.error && response.error->reason == Response::Error::Reason::Connection) {
        // Immediate exponential backoff.
        assert(failedRequests > 0);
        return Seconds(1 << std::min(failedRequests - 1, 31u));
    } else {
        // No error, or not an error that triggers retries.
        return Seconds::max();
    }
}

static Seconds expirationTimeout(const Response& response) {
    // Seconds::zero() is a special value meaning "no expiration".
    if (response.expires > Seconds::zero()) {
        return std::max(Seconds::zero(), response.expires - toSeconds(SystemClock::now()));
    } else {
        return Seconds::max();
    }
}

void OnlineFileRequestImpl::scheduleRealRequest(OnlineFileSource::Impl& impl, bool forceImmediate) {
    if (realRequest) {
        // There's already a request in progress; don't start another one.
        return;
    }

    // If we don't have a fresh response, retry immediately. Otherwise, calculate a timeout
    // that depends on how many consecutive errors we've encountered, and on the expiration time.
    Seconds timeout = (!response || response->stale || forceImmediate)
        ? Seconds::zero()
        : std::min(errorRetryTimeout(*response, failedRequests),
                   expirationTimeout(*response));

    if (timeout == Seconds::max()) {
        return;
    }

    realRequestTimer.start(timeout, Duration::zero(), [this, &impl] {
        assert(!realRequest);
        realRequest = impl.httpContext->createRequest(resource.url, [this, &impl](std::shared_ptr<const Response> response_) {
            realRequest = nullptr;

            if (impl.cache) {
                impl.cache->put(resource, *response_);
            }

            response = response_;

            if (response->error) {
                failedRequests++;
            } else {
                // Reset the number of subsequent failed requests after we got a successful one.
                failedRequests = 0;
            }

            callback(*response);
            scheduleRealRequest(impl);
        }, response);
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
