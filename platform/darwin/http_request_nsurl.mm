#include <mbgl/storage/http_context_base.hpp>
#include <mbgl/storage/http_request_base.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/util/time.hpp>
#include <mbgl/util/parsedate.h>

#import <Foundation/Foundation.h>

#include <map>
#include <cassert>

namespace mbgl {

class HTTPNSURLContext;

class HTTPNSURLRequest : public HTTPRequestBase {
public:
    HTTPNSURLRequest(HTTPNSURLContext*,
                const Resource&,
                Callback,
                uv_loop_t*,
                std::shared_ptr<const Response>);
    ~HTTPNSURLRequest();

    void cancel() final;
    void retry() final;

private:
    void start();
    void handleResult(NSData *data, NSURLResponse *res, NSError *error);
    void handleResponse();
    void retry(uint64_t timeout) final;

    HTTPNSURLContext *context = nullptr;
    bool cancelled = false;
    NSURLSessionDataTask *task = nullptr;
    std::unique_ptr<Response> response;
    const std::shared_ptr<const Response> existingResponse;
    ResponseStatus status = ResponseStatus::PermanentError;
    uv::async async;
    uv::timer timer;
    int attempts = 0;
    enum : bool { PreemptImmediately, ExponentialBackoff } strategy = PreemptImmediately;

    static const int maxAttempts = 4;
};

// -------------------------------------------------------------------------------------------------

class HTTPNSURLContext : public HTTPContextBase {
public:
    HTTPNSURLContext(uv_loop_t *loop);
    ~HTTPNSURLContext();

    HTTPRequestBase* createRequest(const Resource&,
                               RequestBase::Callback,
                               uv_loop_t*,
                               std::shared_ptr<const Response>) final;

    NSURLSession *session = nil;
    NSString *userAgent = nil;
    NSInteger accountType = 0;
};

HTTPNSURLContext::HTTPNSURLContext(uv_loop_t *loop_) : HTTPContextBase(loop_) {
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
        
        accountType = [[NSUserDefaults standardUserDefaults] integerForKey:@"MGLMapboxAccountType"];
    }
}

HTTPNSURLContext::~HTTPNSURLContext() {
    [session release];
    session = nullptr;

    [userAgent release];
    userAgent = nullptr;
}

HTTPRequestBase* HTTPNSURLContext::createRequest(const Resource& resource,
                                             RequestBase::Callback callback,
                                             uv_loop_t* loop,
                                             std::shared_ptr<const Response> response) {
    return new HTTPNSURLRequest(this, resource, callback, loop, response);
}

// -------------------------------------------------------------------------------------------------

HTTPNSURLRequest::HTTPNSURLRequest(HTTPNSURLContext* context_, const Resource& resource_, Callback callback_, uv_loop_t *loop, std::shared_ptr<const Response> existingResponse_)
    : HTTPRequestBase(resource_, callback_),
      context(context_),
      existingResponse(existingResponse_),
      async(loop, [this] { handleResponse(); }),
      timer(loop) {
    context->addRequest(this);
    start();
}

HTTPNSURLRequest::~HTTPNSURLRequest() {
    assert(!task);

    // Stop the backoff timer to avoid re-triggering this request.
    timer.stop();

    context->removeRequest(this);
}

void HTTPNSURLRequest::start() {
    assert(!task);

    attempts++;

    @autoreleasepool {
        NSURL *url = [NSURL URLWithString:@(resource.url.c_str())];
        if (context->accountType == 0 &&
            ([url.host isEqualToString:@"mapbox.com"] || [url.host hasSuffix:@".mapbox.com"])) {
            NSString *absoluteString = [url.absoluteString stringByAppendingFormat:
                                        (url.query ? @"&%@" : @"?%@"), @"events=true"];
            url = [NSURL URLWithString:absoluteString];
        }

        NSMutableURLRequest *req = [NSMutableURLRequest requestWithURL:url];
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
        [task retain];
        [task resume];
    }
}

void HTTPNSURLRequest::handleResponse() {
    if (task) {
        [task release];
        task = nullptr;
    }

    if (!cancelled) {
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
            notify(std::move(response), FileCache::Hint::Refresh);
        } else {
            notify(std::move(response), FileCache::Hint::Full);
        }
    }

    delete this;
}

void HTTPNSURLRequest::cancel() {
    context->removeRequest(this);
    cancelled = true;

    // Stop the backoff timer to avoid re-triggering this request.
    timer.stop();

    if (task) {
        [task cancel];
        [task release];
        task = nullptr;
    } else {
        delete this;
    }
}

void HTTPNSURLRequest::handleResult(NSData *data, NSURLResponse *res, NSError *error) {
    if (error) {
        if ([error code] == NSURLErrorCancelled) {
            status = ResponseStatus::Canceled;
        } else {
            // TODO: Use different codes for host not found, timeout, invalid URL etc.
            // These can be categorized in temporary and permanent errors.
            response = std::make_unique<Response>();
            if (data) {
                response->data = std::make_shared<std::string>((const char *)[data bytes], [data length]);
            }
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

        response = std::make_unique<Response>();
        response->data = std::make_shared<std::string>((const char *)[data bytes], [data length]);

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
        } else if (responseCode == 404) {
            response->status = Response::NotFound;
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
        response = std::make_unique<Response>();
        response->status = Response::Error;
        response->message = "response class is not NSHTTPURLResponse";
    }

    async.send();
}

void HTTPNSURLRequest::retry(uint64_t timeout) {
    response.reset();

    timer.stop();
    timer.start(timeout, 0, [this] { start(); });
}

void HTTPNSURLRequest::retry() {
    // All batons get notified when the network status changed, but some of them
    // might not actually wait for the network to become available again.
    if (strategy == PreemptImmediately && !task) {
        // Triggers the timer upon the next event loop iteration.
        timer.stop();
        timer.start(0, 0, [this] { start(); });
    }
}

std::unique_ptr<HTTPContextBase> HTTPContextBase::createContext(uv_loop_t* loop) {
    return std::make_unique<HTTPNSURLContext>(loop);
}

}
