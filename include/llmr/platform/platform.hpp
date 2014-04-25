#ifndef LLMR_PLATFORM_PLATFORM
#define LLMR_PLATFORM_PLATFORM

#include <memory>
#include <functional>
#include <string>

typedef struct uv_loop_s uv_loop_t;

namespace llmr {

extern const char *kSpriteURL;

namespace platform {

// Restarts painting. This could for example trigger the event loop of the controlling application.
void restart();

class Request;


struct Response {

    int16_t code = -1;
    std::string body;

    std::string error_message;
};

// Makes an HTTP request of a URL on a background thread, calls a function with the results in
// another thread, then calls the foreground callback in the original main thread.
// Returns a cancellable request.
std::shared_ptr<Request> request_http(const std::string &url,
                                      std::function<void(Response *)> background_function,
                                      std::function<void()> foreground_callback,
                                      uv_loop_t *loop);

// Cancels an HTTP request.
void cancel_request_http(const std::shared_ptr<Request> &req);

// Returns a relative timestamp in seconds. This value must be monotonic.
double elapsed();

// Shows an RGBA image with the specified dimensions in a named window.
void show_debug_image(std::string name, const char *data, size_t width, size_t height);

}

}

#endif
