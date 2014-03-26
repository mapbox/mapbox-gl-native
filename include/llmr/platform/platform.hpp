#ifndef LLMR_PLATFORM_IOS
#define LLMR_PLATFORM_IOS

#include <memory>
#include <functional>
#include <string>

namespace llmr {

extern const char *kTileVectorURL;
extern const char *kTileRasterURL;
extern const char *kSpriteURL;
extern const int32_t kTileVectorMaxZoom;
extern const int32_t kTileRasterMaxZoom;

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

// Performs routine cleanup operation and is called on every loop iteration.
void cleanup();

// Shows an RGBA image with the specified dimensions in a named window.
void show_debug_image(std::string name, const char *data, size_t width, size_t height);

}

}

#endif
