#include <mbgl/storage/http_request_handler.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/util/http_header.hpp>
#include <mbgl/util/async_task.hpp>

#include <mbgl/util/version.hpp>

#import <Foundation/Foundation.h>

#include <mutex>

@interface MBGLBundleCanary : NSObject
@end

@implementation MBGLBundleCanary
@end

namespace mbgl {
namespace storage {

// Data that is shared between the requesting thread and the thread running the completion handler.
class HTTPRequestShared {
public:
    HTTPRequestShared(Response& response_, util::AsyncTask& async_)
        : response(response_),
          async(async_) {
    }

    void notify(const Response& response_) {
        std::lock_guard<std::mutex> lock(mutex);
        if (!cancelled) {
            response = response_;
            async.send();
        }
    }

    void cancel() {
        std::lock_guard<std::mutex> lock(mutex);
        cancelled = true;
    }

private:
    std::mutex mutex;
    bool cancelled = false;

    Response& response;
    util::AsyncTask& async;
};

class HTTPRequest : public AsyncRequest {
public:
    HTTPRequest(HTTPRequestHandler::Callback callback_)
        : shared(std::make_shared<HTTPRequestShared>(response, async)),
          callback(callback_) {
    }

    ~HTTPRequest() override {
        shared->cancel();
        if (task) {
            [task cancel];
        }
    }

    std::shared_ptr<HTTPRequestShared> shared;
    NSURLSessionDataTask* task = nil;

private:
    HTTPRequestHandler::Callback callback;
    Response response;

    util::AsyncTask async { [this] {
        // Calling `callback` may result in deleting `this`. Copy data to temporaries first.
        auto callback_ = callback;
        auto response_ = response;
        callback_(response_);
    } };
};

class HTTPRequestHandler::Impl {
public:
    Impl() {
        @autoreleasepool {
            NSURLSessionConfiguration* sessionConfig =
                [NSURLSessionConfiguration defaultSessionConfiguration];
            sessionConfig.timeoutIntervalForResource = 30;
            sessionConfig.HTTPMaximumConnectionsPerHost = 8;
            sessionConfig.requestCachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
            sessionConfig.URLCache = nil;

            session = [NSURLSession sessionWithConfiguration:sessionConfig];

            userAgent = getUserAgent();

            accountType = [[NSUserDefaults standardUserDefaults] integerForKey:@"MGLMapboxAccountType"];
        }
    }

    NSURLSession* session = nil;
    NSString* userAgent = nil;
    NSInteger accountType = 0;
    
private:
    NSString* getUserAgent() const;
    NSBundle* getSDKBundle() const;
};

NSString *HTTPRequestHandler::Impl::getUserAgent() const {
    NSMutableArray *userAgentComponents = [NSMutableArray array];
    
    NSBundle *appBundle = [NSBundle mainBundle];
    if (appBundle) {
        NSString *appName = appBundle.infoDictionary[@"CFBundleName"];
        [userAgentComponents addObject:[NSString stringWithFormat:@"%@/%@",
                                        appName.length ? appName : appBundle.infoDictionary[@"CFBundleIdentifier"],
                                        appBundle.infoDictionary[@"CFBundleShortVersionString"]]];
    } else {
        [userAgentComponents addObject:[NSProcessInfo processInfo].processName];
    }
    
    NSBundle *sdkBundle = HTTPRequestHandler::Impl::getSDKBundle();
    if (sdkBundle) {
        NSString *versionString = sdkBundle.infoDictionary[@"MGLSemanticVersionString"];
        if (!versionString) {
            versionString = sdkBundle.infoDictionary[@"CFBundleShortVersionString"];
        }
        if (versionString) {
            [userAgentComponents addObject:[NSString stringWithFormat:@"%@/%@",
                                            sdkBundle.infoDictionary[@"CFBundleName"], versionString]];
        }
    }
    
    // Avoid %s here because it inserts hidden bidirectional markers on OS X when the system
    // language is set to a right-to-left language.
    [userAgentComponents addObject:[NSString stringWithFormat:@"MapboxGL/%@ (%@)",
                                    CFSTR(MBGL_VERSION_STRING), CFSTR(MBGL_VERSION_REV)]];
    
    NSString *systemName = @"Darwin";
#if TARGET_OS_IPHONE
    systemName = @"iOS";
#elif TARGET_OS_MAC
    systemName = @"OS X";
#elif TARGET_OS_WATCH
    systemName = @"watchOS";
#elif TARGET_OS_TV
    systemName = @"tvOS";
#endif
#if TARGET_OS_SIMULATOR
    systemName = [systemName stringByAppendingString:@" Simulator"];
#endif
    NSString *systemVersion = nil;
    if ([NSProcessInfo instancesRespondToSelector:@selector(operatingSystemVersion)]) {
        NSOperatingSystemVersion osVersion = [NSProcessInfo processInfo].operatingSystemVersion;
        systemVersion = [NSString stringWithFormat:@"%ld.%ld.%ld",
                         (long)osVersion.majorVersion, (long)osVersion.minorVersion, (long)osVersion.patchVersion];
    }
    if (systemVersion) {
        [userAgentComponents addObject:[NSString stringWithFormat:@"%@/%@", systemName, systemVersion]];
    }
    
    NSString *cpu = nil;
#if TARGET_CPU_X86
    cpu = @"x86";
#elif TARGET_CPU_X86_64
    cpu = @"x86_64";
#elif TARGET_CPU_ARM
    cpu = @"arm";
#elif TARGET_CPU_ARM64
    cpu = @"arm64";
#endif
    if (cpu) {
        [userAgentComponents addObject:[NSString stringWithFormat:@"(%@)", cpu]];
    }
    
    return [userAgentComponents componentsJoinedByString:@" "];
}

NSBundle *HTTPRequestHandler::Impl::getSDKBundle() const {
    NSBundle *bundle = [NSBundle bundleForClass:[MBGLBundleCanary class]];
    if (bundle && ![bundle.infoDictionary[@"CFBundlePackageType"] isEqualToString:@"FMWK"]) {
        // For static frameworks, the class is contained in the application bundle rather than the
        // framework bundle.
        bundle = [NSBundle bundleWithPath:[bundle.privateFrameworksPath
                                           stringByAppendingPathComponent:@"Mapbox.framework"]];
    }
    return bundle;
}

HTTPRequestHandler::HTTPRequestHandler()
    : impl(std::make_unique<Impl>()) {
}

HTTPRequestHandler::~HTTPRequestHandler() = default;

uint32_t HTTPRequestHandler::maximumConcurrentRequests() {
    return 20;
}

std::unique_ptr<AsyncRequest> HTTPRequestHandler::request(const Resource& resource, Callback callback) {
    auto request = std::make_unique<HTTPRequest>(callback);
    auto shared = request->shared; // Explicit copy so that it also gets copied into the completion handler block below.

    @autoreleasepool {
        NSURL* url = [NSURL URLWithString:@(resource.url.c_str())];
        if (impl->accountType == 0 &&
            ([url.host isEqualToString:@"mapbox.com"] || [url.host hasSuffix:@".mapbox.com"])) {
            NSString* absoluteString = [url.absoluteString
                stringByAppendingFormat:(url.query ? @"&%@" : @"?%@"), @"events=true"];
            url = [NSURL URLWithString:absoluteString];
        }

        NSMutableURLRequest* req = [NSMutableURLRequest requestWithURL:url];
        if (resource.priorEtag) {
            [req addValue:@(resource.priorEtag->c_str())
                 forHTTPHeaderField:@"If-None-Match"];
        } else if (resource.priorModified) {
            [req addValue:@(util::rfc1123(*resource.priorModified).c_str())
                 forHTTPHeaderField:@"If-Modified-Since"];
        }

        [req addValue:impl->userAgent forHTTPHeaderField:@"User-Agent"];

        request->task = [impl->session
            dataTaskWithRequest:req
              completionHandler:^(NSData* data, NSURLResponse* res, NSError* error) {
                if (error && [error code] == NSURLErrorCancelled) {
                    return;
                }

                Response response;
                using Error = Response::Error;

                if (error) {
                    if (data) {
                        response.data =
                            std::make_shared<std::string>((const char*)[data bytes], [data length]);
                    }

                    switch ([error code]) {
                    case NSURLErrorBadServerResponse: // 5xx errors
                        response.error = std::make_unique<Error>(
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
                        response.error = std::make_unique<Error>(
                            Error::Reason::Connection, [[error localizedDescription] UTF8String]);
                        break;

                    default:
                        response.error = std::make_unique<Error>(
                            Error::Reason::Other, [[error localizedDescription] UTF8String]);
                        break;
                    }
                } else if ([res isKindOfClass:[NSHTTPURLResponse class]]) {
                    const long responseCode = [(NSHTTPURLResponse *)res statusCode];

                    NSDictionary *headers = [(NSHTTPURLResponse *)res allHeaderFields];
                    NSString *cache_control = [headers objectForKey:@"Cache-Control"];
                    if (cache_control) {
                        response.expires = http::CacheControl::parse([cache_control UTF8String]).toTimePoint();
                    }

                    NSString *expires = [headers objectForKey:@"Expires"];
                    if (expires) {
                        response.expires = util::parseTimePoint([expires UTF8String]);
                    }

                    NSString *last_modified = [headers objectForKey:@"Last-Modified"];
                    if (last_modified) {
                        response.modified = util::parseTimePoint([last_modified UTF8String]);
                    }

                    NSString *etag = [headers objectForKey:@"ETag"];
                    if (etag) {
                        response.etag = std::string([etag UTF8String]);
                    }

                    if (responseCode == 200) {
                        response.data = std::make_shared<std::string>((const char *)[data bytes], [data length]);
                    } else if (responseCode == 204 || (responseCode == 404 && resource.kind == Resource::Kind::Tile)) {
                        response.noContent = true;
                    } else if (responseCode == 304) {
                        response.notModified = true;
                    } else if (responseCode == 404) {
                        response.error =
                            std::make_unique<Error>(Error::Reason::NotFound, "HTTP status code 404");
                    } else if (responseCode >= 500 && responseCode < 600) {
                        response.error =
                            std::make_unique<Error>(Error::Reason::Server, std::string{ "HTTP status code " } +
                                                                               std::to_string(responseCode));
                    } else {
                        response.error =
                            std::make_unique<Error>(Error::Reason::Other, std::string{ "HTTP status code " } +
                                                                              std::to_string(responseCode));
                    }
                } else {
                    // This should never happen.
                    response.error = std::make_unique<Error>(Error::Reason::Other,
                                                              "Response class is not NSHTTPURLResponse");
                }

                shared->notify(response);
            }];

        [request->task resume];
    }

    return std::move(request);
}

} // namespace storage
} // namespace mbgl
