#ifndef LLMR_PLATFORM_PLATFORM
#define LLMR_PLATFORM_PLATFORM

#include <memory>
#include <functional>
#include <string>

typedef struct uv_loop_s uv_loop_t;

namespace llmr {

extern const char *kSpriteURL;

namespace platform {

class Request;

struct Response {
    Response(std::function<void(Response *)> callback) : callback(callback) {}
    int16_t code = -1;
    std::string body;
    std::string error_message;
    std::function<void(Response *)> callback;
};

// Makes an HTTP request of a URL, preferrably on a background thread, and calls a function with the
// results in the original thread (which runs the libuv loop).
// Returns a cancellable request.
std::shared_ptr<Request> request_http(const std::string &url,
                                      std::function<void(Response *)> callback, uv_loop_t *loop);

// Cancels an HTTP request.
void cancel_request_http(const std::shared_ptr<Request> &req);

// Shows an alpha image with the specified dimensions in a named window.
void show_debug_image(std::string name, const char *data, size_t width, size_t height);

// Shows an alpha image with the specified dimensions in a named window.
void show_color_debug_image(std::string name, const char *data, size_t width, size_t height);
}
}

#endif
