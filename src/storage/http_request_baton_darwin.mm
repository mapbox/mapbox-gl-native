#include <mbgl/storage/http_request_baton.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/parsedate.h>

#include <uv.h>

#include <mbgl/util/uv.hpp>

#import <Foundation/Foundation.h>
#include <ctime>
#include <xlocale.h>

namespace mbgl {

dispatch_once_t request_initialize = 0;
NSURLSession *session = nullptr;

void HTTPRequestBaton::start() {
    // Starts the request.
    assert(!ptr);

    // Create a C locale
    static locale_t locale = newlocale(LC_ALL_MASK, nullptr, nullptr);

    dispatch_once(&request_initialize, ^{
        NSURLSessionConfiguration *sessionConfig = [NSURLSessionConfiguration defaultSessionConfiguration];
        sessionConfig.timeoutIntervalForResource = 30;
        sessionConfig.HTTPMaximumConnectionsPerHost = 8;
        sessionConfig.requestCachePolicy = NSURLRequestReloadIgnoringLocalCacheData;

        session = [NSURLSession sessionWithConfiguration:sessionConfig];
        // TODO: add a delegate to the session that prohibits caching, since we handle this ourselves.
    });

    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:@(path.c_str())]];
    if (response && response->modified) {
        struct tm *timeinfo;
        char buffer[32];
        const time_t modified = response->modified;
        timeinfo = std::gmtime (&modified);
        strftime_l(buffer, 32 ,"%a, %d %b %Y %H:%M:%S GMT", timeinfo, locale);
        [request addValue:@(buffer) forHTTPHeaderField:@"If-Modified-Since"];
    }

    NSURLSessionDataTask *task = [session dataTaskWithRequest:request
                                            completionHandler:^(NSData *data, NSURLResponse *res, NSError *error) {
        if (error) {
            if ([error code] == NSURLErrorCancelled) {
                // The response code remains at 0 to indicate cancelation.
                // In addition, we don't need any response object.
                response.reset();
                type = HTTPResponseType::Canceled;
            } else {
                // TODO: Use different codes for host not found, timeout, invalid URL etc.
                // These can be categorized in temporary and permanent errors.
                response = std::make_unique<Response>();
                response->code = [(NSHTTPURLResponse *)res statusCode];
                response->message = [[error localizedDescription] UTF8String];

                switch ([error code]) {
                    case NSURLErrorBadServerResponse: // 5xx errors
                        type = HTTPResponseType::TemporaryError;
                        break;

                    case NSURLErrorTimedOut:
                    case NSURLErrorUserCancelledAuthentication:
                        type = HTTPResponseType::SingularError; // retry immediately
                        break;

                    case NSURLErrorNetworkConnectionLost:
                    case NSURLErrorCannotFindHost:
                    case NSURLErrorCannotConnectToHost:
                    case NSURLErrorDNSLookupFailed:
                    case NSURLErrorNotConnectedToInternet:
                    case NSURLErrorInternationalRoamingOff:
                    case NSURLErrorCallIsActive:
                    case NSURLErrorDataNotAllowed:
                        type = HTTPResponseType::ConnectionError;
                        break;

                    default:
                        type = HTTPResponseType::PermanentError;
                }
            }
        } else if ([res isKindOfClass:[NSHTTPURLResponse class]]) {
            const long code = [(NSHTTPURLResponse *)res statusCode];
            if (code == 304) {
                // Assume a Response object already exists.
                assert(response);
            } else {
                response = std::make_unique<Response>();
                response->code = code;
                response->data = {(const char *)[data bytes], [data length]};
            }

            if (code == 304) {
                type = HTTPResponseType::NotModified;
            } else if (code == 200) {
                type = HTTPResponseType::Successful;
            } else {
                assert(!"code must be either 200 or 304");
            }

            NSDictionary *headers = [(NSHTTPURLResponse *)res allHeaderFields];
            NSString *cache_control = [headers objectForKey:@"Cache-Control"];
            if (cache_control) {
                response->expires = Response::parseCacheControl([cache_control UTF8String]);
            }

            NSString *last_modified = [headers objectForKey:@"Last-Modified"];
            if (last_modified) {
                response->modified = parse_date([last_modified UTF8String]);
            }
        } else {
            // This should never happen.
            type = HTTPResponseType::PermanentError;
            response = std::make_unique<Response>();
            response->code = -1;
            response->message = "response class is not NSHTTPURLResponse";
        }

        uv_async_send(async);
    }];

    [task resume];

    ptr = const_cast<void *>(CFBridgingRetain(task));
}

void HTTPRequestBaton::cleanup() {
    if (ptr) {
        CFBridgingRelease(ptr);
        ptr = nullptr;
    }
}

void HTTPRequestBaton::cancel() {
    // After this function returns, the HTTPRequestBaton object may cease to exist at any time.
    // try to stop the request
    if (ptr) {
        NSURLSessionDataTask *task = CFBridgingRelease(ptr);
        ptr = nullptr;
        [task cancel];
    } else {
        // Currently, there is no request in progress. We can delete the async right away.
        uv_async_send(async);
    }
}

}
