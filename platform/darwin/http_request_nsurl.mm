#include <mbgl/storage/http_request.hpp>
#include <mbgl/storage/http_context.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/uv.hpp>

#include <mbgl/util/time.hpp>
#include <mbgl/util/parsedate.h>

#import <Foundation/Foundation.h>

#include <map>
#include <cassert>

namespace mbgl {

enum class ResponseStatus : uint8_t {
    // This error probably won't be resolved by retrying anytime soon. We are giving up.
    PermanentError,

    // This error might be resolved by waiting some time (e.g. server issues).
    // We are going to do an exponential back-off and will try again in a few seconds.
    TemporaryError,

    // This error was caused by a temporary error and it is likely that it will be resolved
    // immediately. We are going to try again right away. This is like the TemporaryError, except
    // that we will not perform exponential back-off.
    SingularError,

    // This error might be resolved once the network reachability status changes.
    // We are going to watch the network status for changes and will retry as soon as the
    // operating system notifies us of a network status change.
    ConnectionError,

    // The request was canceled mid-way.
    Canceled,

    // The request returned data successfully. We retrieved and decoded the data successfully.
    Successful,

    // The request confirmed that the data wasn't changed. We already have the data.
    NotModified,
};

// -------------------------------------------------------------------------------------------------

class HTTPNSURLContext;

class HTTPRequestImpl {
public:
    HTTPRequestImpl(HTTPRequest *request, uv_loop_t *loop, std::shared_ptr<const Response> response);
    ~HTTPRequestImpl();

    void cancel();
    void cancelTimer();

    void start();
    void handleResult(NSData *data, NSURLResponse *res, NSError *error);
    void handleResponse();

    void retry(uint64_t timeout);
    void retryImmediately();
    static void restart(uv_timer_t *timer, int);

private:
    HTTPNSURLContext *context = nullptr;
    HTTPRequest *request = nullptr;
    NSURLSessionDataTask *task = nullptr;
    std::unique_ptr<Response> response;
    const std::shared_ptr<const Response> existingResponse;
    ResponseStatus status = ResponseStatus::PermanentError;
    uv_async_t *async = nullptr;
    int attempts = 0;
    uv_timer_t *timer = nullptr;
    enum : bool { PreemptImmediately, ExponentialBackoff } strategy = PreemptImmediately;

    static const int maxAttempts = 4;
};

// -------------------------------------------------------------------------------------------------

class HTTPNSURLContext : public HTTPContext<HTTPNSURLContext> {
public:
    HTTPNSURLContext(uv_loop_t *loop);
    ~HTTPNSURLContext();

    NSURLSession *session = nil;
    NSString *userAgent = nil;
};

template<> pthread_key_t ThreadContext<HTTPNSURLContext>::key{};
template<> pthread_once_t ThreadContext<HTTPNSURLContext>::once = PTHREAD_ONCE_INIT;

HTTPNSURLContext::HTTPNSURLContext(uv_loop_t *loop_) : HTTPContext(loop_) {
    @autoreleasepool {
        NSURLSessionConfiguration *sessionConfig =
                [NSURLSessionConfiguration defaultSessionConfiguration];
        sessionConfig.timeoutIntervalForResource = 30;
        sessionConfig.HTTPMaximumConnectionsPerHost = 8;
        sessionConfig.requestCachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
        sessionConfig.URLCache = nil;

        session = [NSURLSession sessionWithConfiguration:sessionConfig];
        [session retain];

        // Write user agent string
        userAgent = @"MapboxGL";
    }
}

HTTPNSURLContext::~HTTPNSURLContext() {
    [session release];
    session = nullptr;

    [userAgent release];
    userAgent = nullptr;
}

// -------------------------------------------------------------------------------------------------

HTTPRequestImpl::HTTPRequestImpl(HTTPRequest *request_, uv_loop_t *loop,
                                 std::shared_ptr<const Response> existingResponse_)
    : context(HTTPNSURLContext::Get(loop)),
      request(request_),
      existingResponse(existingResponse_),
      async(new uv_async_t) {
    assert(request);
    context->addRequest(request);

    async->data = this;
    uv_async_init(loop, async, [](uv_async_t *as, int) {
        auto impl = reinterpret_cast<HTTPRequestImpl *>(as->data);
        impl->handleResponse();
    });

    start();
}

void HTTPRequestImpl::start() {
    assert(!task);

    attempts++;

    @autoreleasepool {
        
        NSMutableString *url = [[NSMutableString alloc] initWithString:@(request->resource.url.c_str())];
        if ([[NSUserDefaults standardUserDefaults] objectForKey:@"mapbox_metrics_disabled"] == nil) {
            if ([url rangeOfString:@"?"].location == NSNotFound) {
                [url appendString:@"?"];
            } else {
                [url appendString:@"&"];
            }
            [url appendString:@"events=true"];
        }

        NSMutableURLRequest *req = [[NSMutableURLRequest alloc]
            initWithURL:[NSURL URLWithString:url]];
        if (existingResponse) {
            if (!existingResponse->etag.empty()) {
                [req addValue:@(existingResponse->etag.c_str()) forHTTPHeaderField:@"If-None-Match"];
            } else if (existingResponse->modified) {
                const std::string time = util::rfc1123(existingResponse->modified);
                [req addValue:@(time.c_str()) forHTTPHeaderField:@"If-Modified-Since"];
            }
        }

        [req addValue:context->userAgent forHTTPHeaderField:@"User-Agent"];

        task = [context->session dataTaskWithRequest:req
                          completionHandler:^(NSData *data, NSURLResponse *res,
                                              NSError *error) { handleResult(data, res, error); }];
        [req release];
        [task retain];
        [task resume];
    }
}

void HTTPRequestImpl::handleResponse() {
    if (task) {
        [task release];
        task = nullptr;
    }

    if (request) {
        if (status == ResponseStatus::TemporaryError && attempts < maxAttempts) {
            strategy = ExponentialBackoff;
            return retry((1 << (attempts - 1)) * 1000);
        } else if (status == ResponseStatus::ConnectionError && attempts < maxAttempts) {
            // By default, we will retry every 30 seconds (network change notification will
            // preempt the timeout).
            strategy = PreemptImmediately;
            return retry(30000);
        }

        // Actually return the response.
        if (status == ResponseStatus::NotModified) {
            request->notify(std::move(response), FileCache::Hint::Refresh);
        } else {
            request->notify(std::move(response), FileCache::Hint::Full);
        }

        context->removeRequest(request);
        request->impl = nullptr;
        delete request;
        request = nullptr;
    }

    delete this;
}

void HTTPRequestImpl::cancel() {
    context->removeRequest(request);
    request = nullptr;

    // Stop the backoff timer to avoid re-triggering this request.
    cancelTimer();

    if (task) {
        [task cancel];
        [task release];
        task = nullptr;
    } else {
        delete this;
    }
}

void HTTPRequestImpl::cancelTimer() {
    if (timer) {
        uv_timer_stop(timer);
        uv::close(timer);
        timer = nullptr;
    }
}

HTTPRequestImpl::~HTTPRequestImpl() {
    assert(!task);
    assert(async);

    // Stop the backoff timer to avoid re-triggering this request.
    cancelTimer();

    uv::close(async);

    if (request) {
        context->removeRequest(request);
        request->impl = nullptr;
    }
}

int64_t parseCacheControl(const char *value) {
    if (value) {
        unsigned long long seconds = 0;
        // TODO: cache-control may contain other information as well:
        // http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.9
        if (std::sscanf(value, "max-age=%llu", &seconds) == 1) {
            return std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::system_clock::now().time_since_epoch()).count() +
                   seconds;
        }
    }

    return 0;
}

void HTTPRequestImpl::handleResult(NSData *data, NSURLResponse *res, NSError *error) {
    if (error) {
        if ([error code] == NSURLErrorCancelled) {
            status = ResponseStatus::Canceled;
        } else {
            // TODO: Use different codes for host not found, timeout, invalid URL etc.
            // These can be categorized in temporary and permanent errors.
            response = util::make_unique<Response>();
            response->status = Response::Error;
            response->message = [[error localizedDescription] UTF8String];

            switch ([error code]) {
                case NSURLErrorBadServerResponse: // 5xx errors
                    status = ResponseStatus::TemporaryError;
                    break;

                case NSURLErrorTimedOut:
                case NSURLErrorUserCancelledAuthentication:
                    status = ResponseStatus::SingularError; // retry immediately
                    break;

                case NSURLErrorNetworkConnectionLost:
                case NSURLErrorCannotFindHost:
                case NSURLErrorCannotConnectToHost:
                case NSURLErrorDNSLookupFailed:
                case NSURLErrorNotConnectedToInternet:
                case NSURLErrorInternationalRoamingOff:
                case NSURLErrorCallIsActive:
                case NSURLErrorDataNotAllowed:
                    status = ResponseStatus::ConnectionError;
                    break;

                default:
                    status = ResponseStatus::PermanentError;
            }
        }
    } else if ([res isKindOfClass:[NSHTTPURLResponse class]]) {
        const long responseCode = [(NSHTTPURLResponse *)res statusCode];

        response = util::make_unique<Response>();
        response->data = {(const char *)[data bytes], [data length]};

        NSDictionary *headers = [(NSHTTPURLResponse *)res allHeaderFields];
        NSString *cache_control = [headers objectForKey:@"Cache-Control"];
        if (cache_control) {
            response->expires = parseCacheControl([cache_control UTF8String]);
        }

        NSString *expires = [headers objectForKey:@"Expires"];
        if (expires) {
            response->expires = parse_date([expires UTF8String]);
        }

        NSString *last_modified = [headers objectForKey:@"Last-Modified"];
        if (last_modified) {
            response->modified = parse_date([last_modified UTF8String]);
        }

        NSString *etag = [headers objectForKey:@"ETag"];
        if (etag) {
            response->etag = [etag UTF8String];
        }

        if (responseCode == 304) {
            if (existingResponse) {
                // We're going to copy over the existing response's data.
                response->status = existingResponse->status;
                response->message = existingResponse->message;
                response->modified = existingResponse->modified;
                response->etag = existingResponse->etag;
                response->data = existingResponse->data;
                status = ResponseStatus::NotModified;
            } else {
                // This is an unsolicited 304 response and should only happen on malfunctioning
                // HTTP servers. It likely doesn't include any data, but we don't have much options.
                response->status = Response::Successful;
                status = ResponseStatus::Successful;
            }
        } else if (responseCode == 200) {
            response->status = Response::Successful;
            status = ResponseStatus::Successful;
        } else if (responseCode >= 500 && responseCode < 600) {
            // Server errors may be temporary, so back off exponentially.
            response->status = Response::Error;
            response->message = "HTTP status code " + std::to_string(responseCode);
            status = ResponseStatus::TemporaryError;
        } else {
            // We don't know how to handle any other errors, so declare them as permanently failing.
            response->status = Response::Error;
            response->message = "HTTP status code " + std::to_string(responseCode);
            status = ResponseStatus::PermanentError;
        }
    } else {
        // This should never happen.
        status = ResponseStatus::PermanentError;
        response = util::make_unique<Response>();
        response->status = Response::Error;
        response->message = "response class is not NSHTTPURLResponse";
    }

    uv_async_send(async);
}

void HTTPRequestImpl::retry(uint64_t timeout) {
    response.reset();

    assert(!timer);
    timer = new uv_timer_t;
    timer->data = this;
    uv_timer_init(async->loop, timer);
    uv_timer_start(timer, restart, timeout, 0);
}

void HTTPRequestImpl::retryImmediately() {
    // All batons get notified when the network status changed, but some of them
    // might not actually wait for the network to become available again.
    if (timer && strategy == PreemptImmediately) {
        // Triggers the timer upon the next event loop iteration.
        uv_timer_stop(timer);
        uv_timer_start(timer, restart, 0, 0);
    }
}

void HTTPRequestImpl::restart(uv_timer_t *timer, int) {
    // Restart the request.
    auto impl = reinterpret_cast<HTTPRequestImpl *>(timer->data);

    // Get rid of the timer.
    impl->timer = nullptr;
    uv::close(timer);

    impl->start();
}

// -------------------------------------------------------------------------------------------------

HTTPRequest::HTTPRequest(const Resource& resource, Callback callback,
                         uv_loop_t* loop, std::shared_ptr<const Response> response)
    : RequestBase(resource, callback)
    , impl(new HTTPRequestImpl(this, loop, response)) {
}

HTTPRequest::~HTTPRequest() {
    if (impl) {
        impl->cancel();
    }
}

void HTTPRequest::retryImmediately() {
    if (impl) {
        impl->retryImmediately();
    }
}

void HTTPRequest::cancel() {
    delete this;
}

}
