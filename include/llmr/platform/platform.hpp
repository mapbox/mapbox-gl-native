#ifndef LLMR_PLATFORM_IOS
#define LLMR_PLATFORM_IOS

#include <memory>
#include <functional>
#include <string>

#define kTileURL "http://a.gl-api-us-east-1.tilestream.net/v3/mapbox.mapbox-streets-v4/%d/%d/%d.gl.pbf"
#define kSpriteURL "http://mapbox-kkaefer.s3.amazonaws.com/static/sprite"

namespace llmr {

class Tile;

typedef std::shared_ptr<Tile> tile_ptr;

namespace platform {

// Restarts painting. This could for example trigger the event loop of the controlling application.
void restart(void *obj);

struct Response {
    int16_t code = -1;
    std::string body;
};

// Makes an HTTP request of a URL on a background thread, calls a function with the results on the same thread, and finally calls a callback function on the main thread.
void request_http(std::string url, std::function<void(Response&)> background_function, std::function<void()> foreground_callback);

// Makes a tile-specific HTTP request of a URL on a background thread, calls a function with the results on the same thread, and finally calls a callback function on the main thread. Access to the tile object is provided via a shared pointer.
void request_http_tile(std::string tile_template_url, tile_ptr tile_object, std::function<void(Response&)> background_function, std::function<void()> foreground_callback);

// Returns a relative timestamp in seconds. This value must be monotonic.
double time();

}

}

#endif
