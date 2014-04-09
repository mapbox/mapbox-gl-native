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

class Request;

struct Response {
    int16_t code = -1;
    std::string body;
};

// Makes an HTTP request of a URL on a background thread, calls a function with
// the results in the original threda. Returns a cancellable request.
std::shared_ptr<Request> request_http(const std::string &url, std::function<void(Response *)> fn);

// Cancels an HTTP request.
void cancel_request_http(const std::shared_ptr<Request> &req);

// Shows an RGBA image with the specified dimensions in a named window.
void show_debug_image(std::string name, const char *data, size_t width, size_t height);

}

}

#endif
