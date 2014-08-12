#ifndef MBGL_PLATFORM_PLATFORM
#define MBGL_PLATFORM_PLATFORM

#include <mbgl/util/uv.hpp>

#include <memory>
#include <functional>
#include <string>

namespace mbgl {
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
// If the loop pointer is NULL, the callback function will be called on an arbitrary thread.
// Returns a cancellable request.
std::shared_ptr<Request> request_http(const std::string &url,
                                      std::function<void(Response *)> callback,
                                      std::shared_ptr<uv::loop> loop = nullptr);

// Cancels an HTTP request.
void cancel_request_http(const std::shared_ptr<Request> &req);

// Shows an alpha image with the specified dimensions in a named window.
void show_debug_image(std::string name, const char *data, size_t width, size_t height);

// Shows an alpha image with the specified dimensions in a named window.
void show_color_debug_image(std::string name, const char *data, size_t logical_width, size_t logical_height, size_t width, size_t height);
}
}

#endif
