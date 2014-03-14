#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "settings.hpp"
#include "../common/map_view.cpp"

#include <cstdio>

#include <thread>

MapView *mapView = nullptr;

int main() {
    llmr::Settings_MacOSX settings;
    mapView = new MapView(settings);
    mapView->init();
    int ret = mapView->run();
    mapView->settings.sync();
    delete mapView;
    return ret;
}



namespace llmr {
namespace platform {

void cleanup() {
    // noop
}

void restart() {
    if (mapView) {
        mapView->dirty = true;
        CGEventRef event = CGEventCreate(NULL);
        CGEventSetType(event, kCGEventMouseMoved);
        [[NSApplication sharedApplication] postEvent: [NSEvent eventWithCGEvent:event] atStart:NO];
    }
}

class Request {
public:
    int16_t identifier;
    std::string original_url;
};

Request *request_http(std::string url, std::function<void(Response&)> background_function, std::function<void()> foreground_callback)
{
    NSURLSessionDataTask *task = [[NSURLSession sharedSession] dataTaskWithURL:[NSURL URLWithString:@(url.c_str())] completionHandler:^(NSData *data, NSURLResponse *response, NSError *error)
    {
        Response res;

        if ( ! error && [response isKindOfClass:[NSHTTPURLResponse class]])
        {
            res.code = [(NSHTTPURLResponse *)response statusCode];
            res.body = { (const char *)[data bytes], [data length] };
        }

        background_function(res);

        dispatch_async(dispatch_get_main_queue(), ^(void)
        {
            foreground_callback();
        });
    }];

    [task resume];

    Request *req = new Request();

    req->identifier = task.taskIdentifier;
    req->original_url = url;

    return req;
}

void cancel_request_http(Request *request)
{
    [[NSURLSession sharedSession] getTasksWithCompletionHandler:^(NSArray *dataTasks, NSArray *uploadTasks, NSArray *downloadTasks)
    {
        for (NSURLSessionDownloadTask *task in downloadTasks)
            if (task.taskIdentifier == request->identifier)
                return [task cancel];
    }];
}

double time() {
    return glfwGetTime();
}

}
}
