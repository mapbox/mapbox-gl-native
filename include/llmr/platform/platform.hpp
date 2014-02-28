#ifndef LLMR_PLATFORM_IOS
#define LLMR_PLATFORM_IOS

#include <memory>
#include <functional>
#include <string>

//#define kTileURL "http://a.gl-api-us-east-1.tilestream.net/v3/mapbox.mapbox-streets-v4/%d/%d/%d.gl.pbf"
#define kTileURL "https://a.tiles.mapbox.com/v3/justin.map-pgygbwdm/%d/%d/%d.png256"
#define kSpriteURL "https://dl.dropboxusercontent.com/u/575564/sprite"

namespace llmr {

class Tile;

namespace platform {

// Restarts painting. This could for example trigger the event loop of the controlling application.
void restart();

struct Response {
    int16_t code = -1;
    std::string body;
};

// Makes an HTTP request of a URL on a background thread, calls a function with the results on the same thread, and finally calls a callback function on the main thread.
void request_http(std::string url, std::function<void(Response&)> background_function, std::function<void()> foreground_callback);

// Returns a relative timestamp in seconds. This value must be monotonic.
double time();

}

}

#endif
