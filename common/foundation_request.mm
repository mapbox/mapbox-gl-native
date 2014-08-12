#import "foundation_request.h"

#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#include <atomic>
#endif

#include <memory>
#include <string>
#include <functional>
#include <mbgl/platform/request.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/uv.hpp>

dispatch_once_t request_initialize = 0;
NSURLSession *session = nullptr;

#if TARGET_OS_IPHONE
std::atomic<int> active_tasks;
#endif


// We're using a child class to make sure ARC is working correctly, as well as to add activity
// indicators on iOS.
class FoundationRequest : public mbgl::platform::Request {
public:
    FoundationRequest(const std::string &url,
                      std::function<void(mbgl::platform::Response *)> callback,
                      std::shared_ptr<uv::loop> loop)
        : Request(url, callback, loop) {
#if TARGET_OS_IPHONE
        active_tasks++;
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            [[UIApplication sharedApplication]
                setNetworkActivityIndicatorVisible:(active_tasks > 0)];
        });
#endif
    }

    ~FoundationRequest() {
#if TARGET_OS_IPHONE
        active_tasks--;
        dispatch_async(dispatch_get_main_queue(), ^(void) {
            [[UIApplication sharedApplication]
                setNetworkActivityIndicatorVisible:(active_tasks > 0)];
        });
#endif
    }

    NSURLSessionDataTask *task = nullptr;
};

std::shared_ptr<mbgl::platform::Request>
mbgl::platform::request_http(const std::string &url,
                             std::function<void(Response *)> callback,
                             std::shared_ptr<uv::loop> loop) {
    dispatch_once(&request_initialize, ^{
        NSURLSessionConfiguration *sessionConfig =
            [NSURLSessionConfiguration defaultSessionConfiguration];
        sessionConfig.timeoutIntervalForResource = 30;
        sessionConfig.HTTPMaximumConnectionsPerHost = 8;
        sessionConfig.requestCachePolicy = NSURLRequestUseProtocolCachePolicy;

        session = [NSURLSession sessionWithConfiguration:sessionConfig];

#if TARGET_OS_IPHONE
        active_tasks = 0;
#endif
    });

    std::shared_ptr<FoundationRequest> req =
        std::make_shared<FoundationRequest>(url, callback, loop);

    // Note that we are creating a new shared_ptr pointer(!) to make sure there is at least one
    // shared_ptr in existence while the NSURLSession is loading our data. We are making sure in the
    // callback that this pointer gets destroyed again.
    std::shared_ptr<Request> *req_ptr = new std::shared_ptr<Request>(req);

    NSURLSessionDataTask *task = [session
                                  dataTaskWithURL:[NSURL URLWithString:@(url.c_str())]
                                  completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        if ([error code] == NSURLErrorCancelled) {
            // We intentionally cancelled this request. Make sure we clear the shared_ptr to resolve
            // the circular reference. We're referencing this shared_ptr by value so that the object
            // stays around until this completion handler is invoked.
            delete req_ptr;

            return;
        }

        if (!error && [response isKindOfClass:[NSHTTPURLResponse class]]) {
            (*req_ptr)->res->code = [(NSHTTPURLResponse *)response statusCode];
            (*req_ptr)->res->body = {(const char *)[data bytes], [data length]};
        } else {
            (*req_ptr)->res->error_message = [[error localizedDescription] UTF8String];
        }

        (*req_ptr)->complete();

        delete req_ptr;
    }];

    req->task = task;

    [task resume];
    return req;
}

void mbgl::platform::cancel_request_http(const std::shared_ptr<Request> &req) {
    if (req) {
        [((FoundationRequest *)(req.get()))->task cancel];
    }
}
