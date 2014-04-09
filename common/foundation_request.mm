#import <Foundation/Foundation.h>
// #import <AppKit/AppKit.h>
#include <memory>
#include <string>
#include <functional>
#include <llmr/platform/platform.hpp>

namespace llmr {
namespace platform {

class Request {
public:
    Request(NSURLSessionDataTask *task, const std::string &original_url)
        : task(task), original_url(original_url) {}
    NSURLSessionDataTask *task;
    std::string original_url;
};

std::shared_ptr<Request> request_http(const std::string &url, std::function<void(Response *)> fn) {
    __block std::shared_ptr<Request> req;
    NSURLSessionDataTask *task = [[NSURLSession sharedSession]
                                  dataTaskWithURL:[NSURL URLWithString:@(url.c_str())]
                                  completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        // Make sure we clear the shared_ptr to resolve the circular reference. We're referencing
        // this shared_ptr by value so that the object stays around until this completion handler is
        // invoked.
        req.reset();

        Response res;

        if (!error && [response isKindOfClass:[NSHTTPURLResponse class]]) {
            res.code = [(NSHTTPURLResponse *)response statusCode];
            res.body = {(const char *)[data bytes], [data length]};
        }

        // fprintf(stderr, "loaded\n");
        fn(&res);
    }];

    req = std::make_shared<Request>(task, url);

    [task resume];

    return req;
}

void cancel_request_http(const std::shared_ptr<Request> &req) {
    if (req) {
        [req->task cancel];
    }
}
}
}
