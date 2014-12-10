#include <mbgl/storage/http_request_baton.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/parsedate.h>
#include <mbgl/util/time.hpp>
#include <mbgl/util/version.hpp>

#include <uv.h>

#include <mbgl/util/uv.hpp>

#import <Foundation/Foundation.h>

namespace mbgl {

dispatch_once_t request_initialize = 0;
NSURLSession *session = nullptr;

NSString *userAgent = nil;

void HTTPRequestBaton::start(const util::ptr<HTTPRequestBaton> &ptr) {
    assert(std::this_thread::get_id() == ptr->threadId);

    // Starts the request.
    util::ptr<HTTPRequestBaton> baton = ptr;

    dispatch_once(&request_initialize, ^{
        NSURLSessionConfiguration *sessionConfig = [NSURLSessionConfiguration defaultSessionConfiguration];
        sessionConfig.timeoutIntervalForResource = 30;
        sessionConfig.HTTPMaximumConnectionsPerHost = 8;
        sessionConfig.requestCachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
        sessionConfig.URLCache = nil;

        session = [NSURLSession sessionWithConfiguration:sessionConfig];

        // Write user agent string
        NSDictionary *systemVersion = [NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"];
        userAgent = [NSString stringWithFormat:@"MapboxGL/%d.%d.%d (+https://mapbox.com/mapbox-gl/; %s; %@ %@)",
            version::major, version::minor, version::patch, version::revision,
            [systemVersion objectForKey:@"ProductName"],
            [systemVersion objectForKey:@"ProductVersion"]
        ];
    });

    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:@(baton->path.c_str())]];
    if (baton->response) {
        if (!baton->response->etag.empty()) {
            [request addValue:@(baton->response->etag.c_str()) forHTTPHeaderField:@"If-None-Match"];
        } else if (baton->response->modified) {
            const std::string time = util::rfc1123(baton->response->modified);
            [request addValue:@(time.c_str()) forHTTPHeaderField:@"If-Modified-Since"];
        }
    }

    [request addValue:userAgent forHTTPHeaderField:@"User-Agent"];

    NSURLSessionDataTask *task = [session dataTaskWithRequest:request
                                            completionHandler:^(NSData *data, NSURLResponse *res, NSError *error) {
        if (error) {
            if ([error code] == NSURLErrorCancelled) {
                // The response code remains at 0 to indicate cancelation.
                // In addition, we don't need any response object.
                baton->response.reset();
                baton->type = HTTPResponseType::Canceled;
            } else {
                // TODO: Use different codes for host not found, timeout, invalid URL etc.
                // These can be categorized in temporary and permanent errors.
                baton->response = util::make_unique<Response>();
                baton->response->code = [(NSHTTPURLResponse *)res statusCode];
                baton->response->message = [[error localizedDescription] UTF8String];

                switch ([error code]) {
                    case NSURLErrorBadServerResponse: // 5xx errors
                        baton->type = HTTPResponseType::TemporaryError;
                        break;

                    case NSURLErrorTimedOut:
                    case NSURLErrorUserCancelledAuthentication:
                        baton->type = HTTPResponseType::SingularError; // retry immediately
                        break;

                    case NSURLErrorNetworkConnectionLost:
                    case NSURLErrorCannotFindHost:
                    case NSURLErrorCannotConnectToHost:
                    case NSURLErrorDNSLookupFailed:
                    case NSURLErrorNotConnectedToInternet:
                    case NSURLErrorInternationalRoamingOff:
                    case NSURLErrorCallIsActive:
                    case NSURLErrorDataNotAllowed:
                        baton->type = HTTPResponseType::ConnectionError;
                        break;

                    default:
                        baton->type = HTTPResponseType::PermanentError;
                }
            }
        } else if ([res isKindOfClass:[NSHTTPURLResponse class]]) {
            const long code = [(NSHTTPURLResponse *)res statusCode];
            if (code == 304) {
                // Assume a Response object already exists.
                assert(baton->response);
            } else {
                baton->response = util::make_unique<Response>();
                baton->response->code = code;
                baton->response->data = {(const char *)[data bytes], [data length]};
            }

            if (code == 304) {
                baton->type = HTTPResponseType::NotModified;
            } else if (code == 200) {
                baton->type = HTTPResponseType::Successful;
            } else {
                baton->type = HTTPResponseType::PermanentError;
            }

            NSDictionary *headers = [(NSHTTPURLResponse *)res allHeaderFields];
            NSString *cache_control = [headers objectForKey:@"Cache-Control"];
            if (cache_control) {
                baton->response->expires = Response::parseCacheControl([cache_control UTF8String]);
            }

            NSString *last_modified = [headers objectForKey:@"Last-Modified"];
            if (last_modified) {
                baton->response->modified = parse_date([last_modified UTF8String]);
            }

            NSString *etag = [headers objectForKey:@"ETag"];
            if (etag) {
                baton->response->etag = [etag UTF8String];
            }
        } else {
            // This should never happen.
            baton->type = HTTPResponseType::PermanentError;
            baton->response = util::make_unique<Response>();
            baton->response->code = -1;
            baton->response->message = "response class is not NSHTTPURLResponse";
        }

        uv_async_send(baton->async);
    }];

    [task resume];

    baton->ptr = const_cast<void *>(CFBridgingRetain(task));
}

void HTTPRequestBaton::stop(const util::ptr<HTTPRequestBaton> &ptr) {
    assert(std::this_thread::get_id() == ptr->threadId);
    assert(ptr->ptr);

    NSURLSessionDataTask *task = CFBridgingRelease(ptr->ptr);
    ptr->ptr = nullptr;
    [task cancel];
}

}
