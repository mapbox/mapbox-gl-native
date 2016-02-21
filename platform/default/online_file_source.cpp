#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/http_context_base.hpp>
#include <mbgl/storage/network_status.hpp>

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
#include <list>
#include <unordered_set>
#include <unordered_map>

namespace mbgl {

class OnlineFileRequestImpl : public util::noncopyable {
public:
    using Callback = std::function<void (Response)>;

    OnlineFileRequestImpl(FileRequest*, const Resource&, Callback, OnlineFileSource::Impl&);
    ~OnlineFileRequestImpl();

    void networkIsReachableAgain(OnlineFileSource::Impl&);
    void schedule(OnlineFileSource::Impl&, bool forceImmediate = false);
    void completed(OnlineFileSource::Impl&, Response);

    FileRequest* key;
    Resource resource;
    HTTPRequestBase* request = nullptr;
    util::Timer timer;
    Callback callback;

    // Counts the number of times a response was already expired when received. We're using
    // this to add a delay when making a new request so we don't keep retrying immediately
    // in case of a server serving expired tiles.
    uint32_t expiredRequests = 0;

    // Counts the number of subsequent failed requests. We're using this value for exponential
    // backoff when retrying requests.
    uint32_t failedRequests = 0;
    Response::Error::Reason failedRequestReason = Response::Error::Reason::Success;
};

class OnlineFileSource::Impl {
public:
    // Dummy parameter is a workaround for a gcc 4.9 bug.
    Impl(int) {
        NetworkStatus::Subscribe(&reachability);
    }

    ~Impl() {
        NetworkStatus::Unsubscribe(&reachability);
    }

    void request(FileRequest* key, Resource resource, Callback callback) {
        allRequests[key] = std::make_unique<OnlineFileRequestImpl>(key, resource, callback, *this);
    }

    void cancel(FileRequest* key) {
        allRequests.erase(key);
        if (activeRequests.erase(key)) {
            activatePendingRequest();
        } else {
            auto it = pendingRequestsMap.find(key);
            if (it != pendingRequestsMap.end()) {
                pendingRequestsList.erase(it->second);
                pendingRequestsMap.erase(it);
            }
        }
    }

    void activateOrQueueRequest(OnlineFileRequestImpl* impl) {
        assert(allRequests.find(impl->key) != allRequests.end());
        assert(activeRequests.find(impl->key) == activeRequests.end());
        assert(!impl->request);

        if (activeRequests.size() >= HTTPContextBase::maximumConcurrentRequests()) {
            queueRequest(impl);
        } else {
            activateRequest(impl);
        }
    }

    void queueRequest(OnlineFileRequestImpl* impl) {
        auto it = pendingRequestsList.insert(pendingRequestsList.end(), impl->key);
        pendingRequestsMap.emplace(impl->key, std::move(it));
    }

    void activateRequest(OnlineFileRequestImpl* impl) {
        activeRequests.insert(impl->key);
        impl->request = httpContext->createRequest(impl->resource, [=] (Response response) {
            impl->request = nullptr;
            activeRequests.erase(impl->key);
            activatePendingRequest();
            impl->completed(*this, response);
        });
    }

    void activatePendingRequest() {
        if (pendingRequestsList.empty()) {
            return;
        }

        FileRequest* key = pendingRequestsList.front();
        pendingRequestsList.pop_front();

        pendingRequestsMap.erase(key);

        auto it = allRequests.find(key);
        assert(it != allRequests.end());
        activateRequest(it->second.get());
    }

private:
    void networkIsReachableAgain() {
        for (auto& req : allRequests) {
            req.second->networkIsReachableAgain(*this);
        }
    }

    /**
     * The lifetime of a request is:
     *
     * 1. Waiting for timeout (revalidation or retry)
     * 2. Pending (waiting for room in the active set)
     * 3. Active (open network connection)
     * 4. Back to #1
     *
     * Requests in any state are in `allRequests`. Requests in the pending state are in
     * `pendingRequests`. Requests in the active state are in `activeRequests`.
     */
    std::unordered_map<FileRequest*, std::unique_ptr<OnlineFileRequestImpl>> allRequests;
    std::list<FileRequest*> pendingRequestsList;
    std::unordered_map<FileRequest*, std::list<FileRequest*>::iterator> pendingRequestsMap;
    std::unordered_set<FileRequest*> activeRequests;

    const std::unique_ptr<HTTPContextBase> httpContext { HTTPContextBase::createContext() };
    util::AsyncTask reachability { std::bind(&Impl::networkIsReachableAgain, this) };
};

OnlineFileSource::OnlineFileSource()
    : thread(std::make_unique<util::Thread<Impl>>(
          util::ThreadContext{ "OnlineFileSource", util::ThreadType::Unknown, util::ThreadPriority::Low }, 0)) {
}

OnlineFileSource::~OnlineFileSource() = default;

std::unique_ptr<FileRequest> OnlineFileSource::request(const Resource& resource, Callback callback) {
    Resource res = resource;

    switch (resource.kind) {
    case Resource::Kind::Unknown:
        break;

    case Resource::Kind::Style:
        res.url = mbgl::util::mapbox::normalizeStyleURL(resource.url, accessToken);
        break;

    case Resource::Kind::Source:
        res.url = util::mapbox::normalizeSourceURL(resource.url, accessToken);
        break;

    case Resource::Kind::Glyphs:
        res.url = util::mapbox::normalizeGlyphsURL(resource.url, accessToken);
        break;

    case Resource::Kind::SpriteImage:
    case Resource::Kind::SpriteJSON:
        res.url = util::mapbox::normalizeSpriteURL(resource.url, accessToken);
        break;

    case Resource::Kind::Tile:
        res.url = util::mapbox::normalizeTileURL(resource.url, accessToken);
        break;
    }

    class OnlineFileRequest : public FileRequest {
    public:
        OnlineFileRequest(Resource resource_, FileSource::Callback callback_, util::Thread<OnlineFileSource::Impl>& thread_)
            : thread(thread_),
              workRequest(thread.invokeWithCallback(&OnlineFileSource::Impl::request, callback_, this, resource_)) {
        }

        ~OnlineFileRequest() {
            thread.invoke(&OnlineFileSource::Impl::cancel, this);
        }

        util::Thread<OnlineFileSource::Impl>& thread;
        std::unique_ptr<WorkRequest> workRequest;
    };

    return std::make_unique<OnlineFileRequest>(res, callback, *thread);
}

OnlineFileRequestImpl::OnlineFileRequestImpl(FileRequest* key_, const Resource& resource_, Callback callback_, OnlineFileSource::Impl& impl)
    : key(key_),
      resource(resource_),
      callback(std::move(callback_)) {
    // Force an immediate first request if we don't have an expiration time.
    schedule(impl, !resource.priorExpires);
}

OnlineFileRequestImpl::~OnlineFileRequestImpl() {
    if (request) {
        request->cancel();
    }
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

static Duration expirationTimeout(optional<SystemTimePoint> expires, uint32_t expiredRequests) {
    if (expiredRequests) {
        return Seconds(1 << std::min(expiredRequests - 1, 31u));
    } else if (expires) {
        return std::max(SystemDuration::zero(), *expires - SystemClock::now());
    } else {
        return Duration::max();
    }
}

void OnlineFileRequestImpl::schedule(OnlineFileSource::Impl& impl, bool forceImmediate) {
    if (request) {
        // There's already a request in progress; don't start another one.
        return;
    }

    // If we're not being asked for a forced refresh, calculate a timeout that depends on how many
    // consecutive errors we've encountered, and on the expiration time, if present.
    Duration timeout = forceImmediate
        ? Duration::zero()
        : std::min(errorRetryTimeout(failedRequestReason, failedRequests),
                   expirationTimeout(resource.priorExpires, expiredRequests));

    if (timeout == Duration::max()) {
        return;
    }

    timer.start(timeout, Duration::zero(), [&] {
        impl.activateOrQueueRequest(this);
    });
}

void OnlineFileRequestImpl::completed(OnlineFileSource::Impl& impl, Response response) {
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

    if (response.expires && response.expires < SystemClock::now()) {
        expiredRequests++;
    } else {
        expiredRequests = 0;
    }

    if (!response.etag) {
        response.etag = resource.priorEtag;
    } else {
        resource.priorEtag = response.etag;
    }

    if (response.error) {
        failedRequests++;
        failedRequestReason = response.error->reason;
    } else {
        failedRequests = 0;
        failedRequestReason = Response::Error::Reason::Success;
    }

    callback(response);
    schedule(impl);
}

void OnlineFileRequestImpl::networkIsReachableAgain(OnlineFileSource::Impl& impl) {
    // We need all requests to fail at least once before we are going to start retrying
    // them, and we only immediately restart request that failed due to connection issues.
    if (failedRequestReason == Response::Error::Reason::Connection) {
        schedule(impl, true);
    }
}

} // namespace mbgl
