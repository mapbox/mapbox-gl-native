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

private:
    void handleResult(NSData *data, NSURLResponse *res, NSError *error);
    void handleResponse();

    HTTPNSURLContext *context = nullptr;
    bool cancelled = false;
    NSURLSessionDataTask *task = nullptr;
    std::unique_ptr<Response> response;
    const std::shared_ptr<const Response> existingResponse;
    uv::async async;
};

// -------------------------------------------------------------------------------------------------

class HTTPNSURLContext : public HTTPContextBase {
public:
    HTTPNSURLContext();
    ~HTTPNSURLContext();

    HTTPRequestBase* createRequest(const Resource&,
                               RequestBase::Callback,
                               uv_loop_t*,
                               std::shared_ptr<const Response>) final;

    NSURLSession *session = nil;
    NSString *userAgent = nil;
    NSInteger accountType = 0;
};

HTTPNSURLContext::HTTPNSURLContext() {
    @autoreleasepool {
        NSURLSessionConfiguration* sessionConfig =
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

HTTPNSURLRequest::HTTPNSURLRequest(HTTPNSURLContext* context_,
                                   const Resource& resource_,
                                   Callback callback_,
                                   uv_loop_t* loop,
                                   std::shared_ptr<const Response> existingResponse_)
    : HTTPRequestBase(resource_, callback_),
      context(context_),
      existingResponse(existingResponse_),
      async(loop, [this] { handleResponse(); }) {
    @autoreleasepool {
        NSURL* url = [NSURL URLWithString:@(resource.url.c_str())];
        if (context->accountType == 0 &&
            ([url.host isEqualToString:@"mapbox.com"] || [url.host hasSuffix:@".mapbox.com"])) {
            NSString* absoluteString = [url.absoluteString
                stringByAppendingFormat:(url.query ? @"&%@" : @"?%@"), @"events=true"];
            url = [NSURL URLWithString:absoluteString];
        }

        NSMutableURLRequest* req = [NSMutableURLRequest requestWithURL:url];
        if (existingResponse) {
            if (!existingResponse->etag.empty()) {
                [req addValue:@(existingResponse->etag.c_str())
                    forHTTPHeaderField:@"If-None-Match"];
            } else if (existingResponse->modified) {
                const std::string time = util::rfc1123(existingResponse->modified);
                [req addValue:@(time.c_str()) forHTTPHeaderField:@"If-Modified-Since"];
            }
        }

        [req addValue:context->userAgent forHTTPHeaderField:@"User-Agent"];

        task = [context->session
            dataTaskWithRequest:req
              completionHandler:^(NSData* data, NSURLResponse* res, NSError* error) {
                handleResult(data, res, error);
              }];
        [task retain];
        [task resume];
    }
}

HTTPNSURLRequest::~HTTPNSURLRequest() {
    assert(!task);
}

void HTTPNSURLRequest::handleResponse() {
    if (task) {
        [task release];
        task = nullptr;
    }

    if (!cancelled) {
        // Actually return the response.
        notify(std::move(response));
    }

    delete this;
}

void HTTPNSURLRequest::cancel() {
    cancelled = true;

    if (task) {
        [task cancel];
        [task release];
        task = nullptr;
    } else {
        delete this;
    }
}

void HTTPNSURLRequest::handleResult(NSData *data, NSURLResponse *res, NSError *error) {
    response = std::make_unique<Response>();
    using Error = Response::Error;

    if (error) {
        if ([error code] == NSURLErrorCancelled) {
            response->error =
                std::make_unique<Error>(Error::Reason::Canceled, "Request was cancelled");
        } else {
            if (data) {
                response->data =
                    std::make_shared<std::string>((const char*)[data bytes], [data length]);
            }

            switch ([error code]) {
            case NSURLErrorBadServerResponse: // 5xx errors
                response->error = std::make_unique<Error>(
                    Error::Reason::Server, [[error localizedDescription] UTF8String]);
                break;

            case NSURLErrorNetworkConnectionLost:
            case NSURLErrorCannotFindHost:
            case NSURLErrorCannotConnectToHost:
            case NSURLErrorDNSLookupFailed:
            case NSURLErrorNotConnectedToInternet:
            case NSURLErrorInternationalRoamingOff:
            case NSURLErrorCallIsActive:
            case NSURLErrorDataNotAllowed:
            case NSURLErrorTimedOut:
                response->error = std::make_unique<Error>(
                    Error::Reason::Connection, [[error localizedDescription] UTF8String]);
                break;

            default:
                response->error = std::make_unique<Error>(
                    Error::Reason::Other, [[error localizedDescription] UTF8String]);
                break;
            }
        }
    } else if ([res isKindOfClass:[NSHTTPURLResponse class]]) {
        const long responseCode = [(NSHTTPURLResponse *)res statusCode];

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

        if (responseCode == 200) {
            // Nothing to do; this is what we want.
        } else if (responseCode == 304) {
            if (existingResponse) {
                // We're going to copy over the existing response's data.
                if (existingResponse->error) {
                    response->error = std::make_unique<Error>(*existingResponse->error);
                }
                response->data = existingResponse->data;
                response->modified = existingResponse->modified;
                // We're not updating `expired`, it was probably set during the request.
                response->etag = existingResponse->etag;
            } else {
                // This is an unsolicited 304 response and should only happen on malfunctioning
                // HTTP servers. It likely doesn't include any data, but we don't have much options.
            }
        } else if (responseCode == 404) {
            response->error =
                std::make_unique<Error>(Error::Reason::NotFound, "HTTP status code 404");
        } else if (responseCode >= 500 && responseCode < 600) {
            response->error =
                std::make_unique<Error>(Error::Reason::Server, std::string{ "HTTP status code " } +
                                                                   std::to_string(responseCode));
        } else {
            response->error =
                std::make_unique<Error>(Error::Reason::Other, std::string{ "HTTP status code " } +
                                                                  std::to_string(responseCode));
        }
    } else {
        // This should never happen.
        response->error = std::make_unique<Error>(Error::Reason::Other,
                                                  "Response class is not NSHTTPURLResponse");
    }

    async.send();
}

std::unique_ptr<HTTPContextBase> HTTPContextBase::createContext(uv_loop_t*) {
    return std::make_unique<HTTPNSURLContext>();
}

}
