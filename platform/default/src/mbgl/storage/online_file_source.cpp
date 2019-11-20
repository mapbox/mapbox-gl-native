#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/storage/network_status.hpp>

#include <mbgl/storage/resource_transform.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/logging.hpp>

#include <mbgl/actor/mailbox.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>
#include <mbgl/util/http_timeout.hpp>

#include <algorithm>
#include <cassert>
#include <list>
#include <unordered_set>
#include <unordered_map>

namespace mbgl {

static uint32_t DEFAULT_MAXIMUM_CONCURRENT_REQUESTS = 20;

class OnlineFileRequest : public AsyncRequest {
public:
    using Callback = std::function<void (Response)>;

    OnlineFileRequest(Resource, Callback, OnlineFileSource::Impl&);
    ~OnlineFileRequest() override;

    void networkIsReachableAgain();
    void schedule();
    void schedule(optional<Timestamp> expires);
    void completed(Response);

    void setTransformedURL(const std::string&& url);
    ActorRef<OnlineFileRequest> actor();

    OnlineFileSource::Impl& impl;
    Resource resource;
    std::unique_ptr<AsyncRequest> request;
    util::Timer timer;
    Callback callback;

    std::shared_ptr<Mailbox> mailbox;

    // Counts the number of times a response was already expired when received. We're using
    // this to add a delay when making a new request so we don't keep retrying immediately
    // in case of a server serving expired tiles.
    uint32_t expiredRequests = 0;

    // Counts the number of subsequent failed requests. We're using this value for exponential
    // backoff when retrying requests.
    uint32_t failedRequests = 0;
    Response::Error::Reason failedRequestReason = Response::Error::Reason::Success;
    optional<Timestamp> retryAfter;
};

class OnlineFileSource::Impl {
public:
    Impl() {
        NetworkStatus::Subscribe(&reachability);
        setMaximumConcurrentRequests(DEFAULT_MAXIMUM_CONCURRENT_REQUESTS);
    }

    ~Impl() {
        NetworkStatus::Unsubscribe(&reachability);
    }

    void add(OnlineFileRequest* request) {
        allRequests.insert(request);
        if (resourceTransform) {
            // Request the ResourceTransform actor a new url and replace the resource url with the
            // transformed one before proceeding to schedule the request.
            resourceTransform->invoke(&ResourceTransform::transform,
                                      request->resource.kind,
                                      std::move(request->resource.url),
                                      [ref = request->actor()](const std::string&& url) {
                                          ref.invoke(&OnlineFileRequest::setTransformedURL, url);
                                      });
        } else {
            request->schedule();
        }
    }

    void remove(OnlineFileRequest* request) {
        allRequests.erase(request);
        if (activeRequests.erase(request)) {
            activatePendingRequest();
        } else {
            pendingRequests.remove(request);
        }
    }

    void activateOrQueueRequest(OnlineFileRequest* request) {
        assert(allRequests.find(request) != allRequests.end());
        assert(activeRequests.find(request) == activeRequests.end());
        assert(!request->request);

        if (activeRequests.size() >= getMaximumConcurrentRequests()) {
            queueRequest(request);
        } else {
            activateRequest(request);
        }
    }

    void queueRequest(OnlineFileRequest* request) {
        pendingRequests.insert(request);
    }

    void activateRequest(OnlineFileRequest* request) {
        auto callback = [=](Response response) {
            activeRequests.erase(request);
            request->request.reset();
            request->completed(response);
            activatePendingRequest();
        };

        activeRequests.insert(request);

        if (online) {
            request->request = httpFileSource.request(request->resource, callback);
        } else {
            Response response;
            response.error = std::make_unique<Response::Error>(Response::Error::Reason::Connection,
                                                               "Online connectivity is disabled.");
            callback(response);
        }

    }

    void activatePendingRequest() {

        auto request = pendingRequests.pop();

        if (request) {
            activateRequest(*request);
        }
    }

    bool isPending(OnlineFileRequest* request) {
        return pendingRequests.contains(request);
    }

    bool isActive(OnlineFileRequest* request) {
        return activeRequests.find(request) != activeRequests.end();
    }

    void setResourceTransform(optional<ActorRef<ResourceTransform>>&& transform) {
        resourceTransform = std::move(transform);
    }

    void setOnlineStatus(const bool status) {
        online = status;
        if (online) {
            networkIsReachableAgain();
        }
    }

    uint32_t getMaximumConcurrentRequests() const {
        return maximumConcurrentRequests;
    }

    void setMaximumConcurrentRequests(uint32_t maximumConcurrentRequests_) {
        maximumConcurrentRequests = maximumConcurrentRequests_;
    }

private:

    void networkIsReachableAgain() {
        // Notify regular priority requests.
        for (auto& request : allRequests) {
            if (request->resource.priority == Resource::Priority::Regular) {
                request->networkIsReachableAgain();
            }
        }

        // Notify low priority requests.
        for (auto& request : allRequests) {
            if (request->resource.priority == Resource::Priority::Low) {
                request->networkIsReachableAgain();
            }
        }
    }

    // Using Pending Requests as an priority queue which processes
    // file requests in a FIFO manner but prefers regular requests
    // over offline requests with a low priority such that low priority
    // requests do not throttle regular requests.
    //
    // The order of a queue is therefore:
    //
    // hi0 -- hi1 -- hi2 -- hi3 -- lo0 -- lo1 --lo2
    //                              ^
    //                              firstLowPriorityRequest

    struct PendingRequests {
        PendingRequests() : queue(), firstLowPriorityRequest(queue.begin()) {}

        std::list<OnlineFileRequest*> queue;
        std::list<OnlineFileRequest*>::iterator firstLowPriorityRequest;

        void remove(const OnlineFileRequest* request) {
            auto it = std::find(queue.begin(), queue.end(), request);
            if (it != queue.end()) {
                if (it == firstLowPriorityRequest) {
                    firstLowPriorityRequest++;
                }
                queue.erase(it);
            }
        }

        void insert(OnlineFileRequest* request) {
            if (request->resource.priority == Resource::Priority::Regular) {
                firstLowPriorityRequest = queue.insert(firstLowPriorityRequest, request);
                firstLowPriorityRequest++;
            }
            else {
                if (firstLowPriorityRequest == queue.end()) {
                    firstLowPriorityRequest = queue.insert(queue.end(), request);
                }
                else {
                    queue.insert(queue.end(), request);
                }
            }
        }


        optional<OnlineFileRequest*> pop() {
            if (queue.empty()) {
                return optional<OnlineFileRequest*>();
            }

            if (queue.begin() == firstLowPriorityRequest) {
                firstLowPriorityRequest++;
            }

            OnlineFileRequest* next = queue.front();
            queue.pop_front();
            return optional<OnlineFileRequest*>(next);
        }

        bool contains(OnlineFileRequest* request) const {
            return (std::find(queue.begin(), queue.end(), request) != queue.end());
        }

    };

    optional<ActorRef<ResourceTransform>> resourceTransform;

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
    std::unordered_set<OnlineFileRequest*> allRequests;

    PendingRequests pendingRequests;

    std::unordered_set<OnlineFileRequest*> activeRequests;

    bool online = true;
    uint32_t maximumConcurrentRequests;
    HTTPFileSource httpFileSource;
    util::AsyncTask reachability { std::bind(&Impl::networkIsReachableAgain, this) };
};

OnlineFileSource::OnlineFileSource()
    : impl(std::make_unique<Impl>()) {
}

OnlineFileSource::~OnlineFileSource() = default;

std::unique_ptr<AsyncRequest> OnlineFileSource::request(const Resource& resource, Callback callback) {
    Resource res = resource;

    switch (resource.kind) {
    case Resource::Kind::Unknown:
    case Resource::Kind::Image:
        break;

    case Resource::Kind::Style:
        res.url = mbgl::util::mapbox::normalizeStyleURL(apiBaseURL, resource.url, accessToken);
        break;

    case Resource::Kind::Source:
        res.url = util::mapbox::normalizeSourceURL(apiBaseURL, resource.url, accessToken);
        break;

    case Resource::Kind::Glyphs:
        res.url = util::mapbox::normalizeGlyphsURL(apiBaseURL, resource.url, accessToken);
        break;

    case Resource::Kind::SpriteImage:
    case Resource::Kind::SpriteJSON:
        res.url = util::mapbox::normalizeSpriteURL(apiBaseURL, resource.url, accessToken);
        break;

    case Resource::Kind::Tile:
        res.url = util::mapbox::normalizeTileURL(apiBaseURL, resource.url, accessToken);
        break;
    }

    return std::make_unique<OnlineFileRequest>(std::move(res), std::move(callback), *impl);
}

void OnlineFileSource::setResourceTransform(optional<ActorRef<ResourceTransform>>&& transform) {
    impl->setResourceTransform(std::move(transform));
}

OnlineFileRequest::OnlineFileRequest(Resource resource_, Callback callback_, OnlineFileSource::Impl& impl_)
    : impl(impl_),
      resource(std::move(resource_)),
      callback(std::move(callback_)) {
    impl.add(this);
}

void OnlineFileRequest::schedule() {
    // Force an immediate first request if we don't have an expiration time.
    if (resource.priorExpires) {
        schedule(resource.priorExpires);
    } else {
        schedule(util::now());
    }
}

OnlineFileRequest::~OnlineFileRequest() {
    impl.remove(this);
}

Timestamp interpolateExpiration(const Timestamp& current,
                                optional<Timestamp> prior,
                                bool& expired) {
    auto now = util::now();
    if (current > now) {
        return current;
    }

    if (!bool(prior)) {
        expired = true;
        return current;
    }

    // Expiring date is going backwards,
    // fallback to exponential backoff.
    if (current < *prior) {
        expired = true;
        return current;
    }

    auto delta = current - *prior;

    // Server is serving the same expired resource
    // over and over, fallback to exponential backoff.
    if (delta == Duration::zero()) {
        expired = true;
        return current;
    }

    // Assume that either the client or server clock is wrong and
    // try to interpolate a valid expiration date (from the client POV)
    // observing a minimum timeout.
    return now + std::max<Seconds>(delta, util::CLOCK_SKEW_RETRY_TIMEOUT);
}

void OnlineFileRequest::schedule(optional<Timestamp> expires) {
    if (impl.isPending(this) || impl.isActive(this)) {
        // There's already a request in progress; don't start another one.
        return;
    }

    // If we're not being asked for a forced refresh, calculate a timeout that depends on how many
    // consecutive errors we've encountered, and on the expiration time, if present.
    Duration timeout = std::min(
                            http::errorRetryTimeout(failedRequestReason, failedRequests, retryAfter),
                            http::expirationTimeout(expires, expiredRequests));

    if (timeout == Duration::max()) {
        return;
    }

    // Emulate a Connection error when the Offline mode is forced with
    // a really long timeout. The request will get re-triggered when
    // the NetworkStatus is set back to Online.
    if (NetworkStatus::Get() == NetworkStatus::Status::Offline) {
        failedRequestReason = Response::Error::Reason::Connection;
        failedRequests = 1;
        timeout = Duration::max();
    }

    timer.start(timeout, Duration::zero(), [&] {
        impl.activateOrQueueRequest(this);
    });
}

void OnlineFileRequest::completed(Response response) {
    // If we didn't get various caching headers in the response, continue using the
    // previous values. Otherwise, update the previous values to the new values.

    if (!response.modified) {
        response.modified = resource.priorModified;
    } else {
        resource.priorModified = response.modified;
    }

    if (response.notModified && resource.priorData) {
        // When the priorData field is set, it indicates that we had to revalidate the request and
        // that the requestor hasn't gotten data yet. If we get a 304 response, this means that we
        // have send the cached data to give the requestor a chance to actually obtain the data.
        response.data = std::move(resource.priorData);
        response.notModified = false;
    }

    bool isExpired = false;

    if (response.expires) {
        auto prior = resource.priorExpires;
        resource.priorExpires = response.expires;
        response.expires = interpolateExpiration(*response.expires, prior, isExpired);
    }

    if (isExpired) {
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
        retryAfter = response.error->retryAfter;
    } else {
        failedRequests = 0;
        failedRequestReason = Response::Error::Reason::Success;
    }

    schedule(response.expires);

    // Calling the callback may result in `this` being deleted. It needs to be done last,
    // and needs to make a local copy of the callback to ensure that it remains valid for
    // the duration of the call.
    auto callback_ = callback;
    callback_(response);
}

void OnlineFileRequest::networkIsReachableAgain() {
    // We need all requests to fail at least once before we are going to start retrying
    // them, and we only immediately restart request that failed due to connection issues.
    if (failedRequestReason == Response::Error::Reason::Connection) {
        schedule(util::now());
    }
}

void OnlineFileRequest::setTransformedURL(const std::string&& url) {
    resource.url = url;
    schedule();
}

ActorRef<OnlineFileRequest> OnlineFileRequest::actor() {
    if (!mailbox) {
        // Lazy constructed because this can be costly and
        // the ResourceTransform is not used by many apps.
        mailbox = std::make_shared<Mailbox>(*Scheduler::GetCurrent());
    }

    return ActorRef<OnlineFileRequest>(*this, mailbox);
}

void OnlineFileSource::setMaximumConcurrentRequests(uint32_t maximumConcurrentRequests_) {
    impl->setMaximumConcurrentRequests(maximumConcurrentRequests_);
}

uint32_t OnlineFileSource::getMaximumConcurrentRequests() const {
    return impl->getMaximumConcurrentRequests();
}


// For testing only:

void OnlineFileSource::setOnlineStatus(const bool status) {
    impl->setOnlineStatus(status);
}

} // namespace mbgl
