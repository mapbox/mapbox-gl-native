#ifndef LLMR_PLATFORM_IOS
#define LLMR_PLATFORM_IOS

#include <memory>
#include <functional>
#include <string>

namespace llmr {

class Tile;

namespace platform {

// Restarts painting. This could for example trigger the event loop of the
// controlling application.
void restart(void *obj);

// Download a tile. Obtain the z/x/y from the tile shared_ptr.
// You should perform the download and parsing of the tile in a separate thread!
// Then, call map.tileLoaded(tile); or map.tileFailed(tile); in the main thread.
void request(void *obj, std::shared_ptr<Tile> tile);


struct Response {
    Response(int16_t code, const char *body, size_t length)
        : code(code),
          body(body),
          length(length) {}
    int16_t code;
    const char *body;
    size_t length;
};

void request_http(std::string url, std::function<void(const Response&)> func);

// Returns a relative timestamp in seconds. This value must be monotonic.
double time();

}

}

#endif
