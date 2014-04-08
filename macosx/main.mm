#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "settings.hpp"
#include "../common/glfw_view.hpp"
#include "settings.hpp"

#include <cstdio>


int main() {
    llmr::Settings_MacOSX settings;
    llmr::Map map(settings);
    llmr::GLFWView view(map);
    view.show();
    settings.sync();
    return 0;
}



namespace llmr {
namespace platform {

class Request {
public:
    uint64_t identifier;
    std::string original_url;
};

Request *request_http(std::string url, std::function<void(Response *)> fn) {
    fprintf(stderr, "[request_http request thread: %lx]\n", uv_thread_self());
    NSURLSessionDataTask *task = [[NSURLSession sharedSession]
                                  dataTaskWithURL:[NSURL URLWithString:@(url.c_str())]
                                  completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        Response *res = new Response();

        if (!error && [response isKindOfClass:[NSHTTPURLResponse class]]) {
            res->code = [(NSHTTPURLResponse *)response statusCode];
            res->body = { (const char *)[data bytes], [data length] };
        }

        fn(res);
    }];

    [task resume];

    return new Request({
        task.taskIdentifier,
        url
    });;
}

void cancel_request_http(Request *request) {
    [[NSURLSession sharedSession]
     getTasksWithCompletionHandler:^(NSArray *dataTasks, NSArray *uploadTasks, NSArray *downloadTasks) {
        for (NSURLSessionDownloadTask *task in downloadTasks) {
            if (task.taskIdentifier == request->identifier) {
                return [task cancel];
            }
        }
    }];
}

}
}
