#ifndef LLMR_PLATFORM_IOS
#define LLMR_PLATFORM_IOS

#include <memory>
#include <functional>
#include <string>

#define kTileURL "http://a.gl-api-us-east-1.tilestream.net/v3/mapbox.mapbox-streets-v4/%d/%d/%d.gl.pbf"
#define kSpriteURL "https://dl.dropboxusercontent.com/u/575564/sprite"

namespace llmr {

namespace platform {

// Restarts painting. This could for example trigger the event loop of the controlling application.
void restart();

class Request;

struct Response {
    int16_t code = -1;
    std::string body;
};

// Makes an HTTP request of a URL on a background thread, calls a function with the results on the same thread, and finally calls a callback function on the main thread. Returns a cancellable request.
Request *request_http(std::string url, std::function<void(Response&)> background_function, std::function<void()> foreground_callback);

// Cancels an HTTP request.
void cancel_request_http(Request *request);

// Returns a relative timestamp in seconds. This value must be monotonic.
double time();

}

}

#endif
