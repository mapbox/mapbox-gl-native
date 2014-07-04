#ifndef LLMR_PLATFORM_PLATFORM
#define LLMR_PLATFORM_PLATFORM

#include <memory>
#include <functional>
#include <string>

typedef struct uv_loop_s uv_loop_t;

namespace uv {
class loop;
}

namespace llmr {
namespace platform {

class Request;

struct Response {
    Response(std::function<void(Response *)> callback) : callback(callback) {}
    int16_t code = -1;
    std::string body;
    std::string error_message;
    std::function<void(Response *)> callback;
};

enum MapChange : uint8_t {
    MapChangeRegionWillChange = 0,
    MapChangeRegionWillChangeAnimated = 1,
    MapChangeRegionDidChange = 2,
    MapChangeRegionDidChangeAnimated = 3,
    MapChangeWillStartLoadingMap = 4,
    MapChangeDidFinishLoadingMap = 5,
    MapChangeDidFailLoadingMap = 6,
    MapChangeWillStartRenderingMap = 7,
    MapChangeDidFinishRenderingMap = 8,
    MapChangeDidFinishRenderingMapFullyRendered = 9
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

// Notifies a watcher of map x/y/scale/rotation changes.
void notify_map_change();

// Shows an alpha image with the specified dimensions in a named window.
void show_debug_image(std::string name, const char *data, size_t width, size_t height);

// Shows an alpha image with the specified dimensions in a named window.
void show_color_debug_image(std::string name, const char *data, size_t logical_width, size_t logical_height, size_t width, size_t height);
}
}

#endif
