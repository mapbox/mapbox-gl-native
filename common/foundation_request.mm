#import "foundation_request.h"

#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#include <atomic>
#endif

#include <memory>
#include <string>
#include <functional>
#include <llmr/platform/request.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/util/std.hpp>
#include <uv.h>

uv_once_t request_initialize = UV_ONCE_INIT;
NSURLSession *session = nullptr;

#if TARGET_OS_IPHONE
std::atomic<int> active_tasks;
#endif

void request_initialize_cb() {
    NSURLSessionConfiguration *sessionConfig =
        [NSURLSessionConfiguration defaultSessionConfiguration];
    sessionConfig.timeoutIntervalForResource = 6;
    sessionConfig.HTTPMaximumConnectionsPerHost = 8;
    sessionConfig.requestCachePolicy = NSURLRequestUseProtocolCachePolicy;

    session = [NSURLSession sessionWithConfiguration:sessionConfig];

#if TARGET_OS_IPHONE
    active_tasks = 0;
#endif
}

// We're using a child class to make sure ARC is working correctly, as well as to add activity
// indicators on iOS.
class FoundationRequest : public llmr::platform::Request {
public:
    FoundationRequest(const std::string &url,
                      std::function<void(llmr::platform::Response *)> background_function,
                      std::function<void()> foreground_callback)
        : Request(url, background_function, foreground_callback) {
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

std::shared_ptr<llmr::platform::Request>
llmr::platform::request_http(const std::string &url,
                             std::function<void(Response *)> background_function,
                             std::function<void()> foreground_callback) {
    uv_once(&request_initialize, request_initialize_cb);

    std::shared_ptr<FoundationRequest> req =
        std::make_shared<FoundationRequest>(url, background_function, foreground_callback);

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

        req->res = std::make_unique<Response>();

        if (!error && [response isKindOfClass:[NSHTTPURLResponse class]]) {
            req->res->code = [(NSHTTPURLResponse *)response statusCode];
            req->res->body = {(const char *)[data bytes], [data length]};
        } else {
            req->res->error_message = [[error localizedDescription] UTF8String];
        }

        // We're currently in the request thread. We're going to schedule a uv_work request that
        // executes the background function in a threadpool, and tell it to call the after callback
        // back in the main uv loop.
        uv_work_t *work = new uv_work_t();

        // We're passing on the pointer we created to the work structure. It is going to be deleted
        // in the after_work_cb;
        work->data = req_ptr;

        // Executes the background_function in a libuv thread pool, and the after_work_cb back in
        // the *main* event loop.
        uv_queue_work(uv_default_loop(), work, Request::work_callback,
                      Request::after_work_callback);
    }];

    req->task = task;

    [task resume];
    return req;
}

void llmr::platform::cancel_request_http(const std::shared_ptr<Request> &req) {
    if (req) {
        [((FoundationRequest *)(req.get()))->task cancel];
    }
}
