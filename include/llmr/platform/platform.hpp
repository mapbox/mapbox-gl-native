#ifndef LLMR_PLATFORM_IOS
#define LLMR_PLATFORM_IOS

#include <memory>
#include <functional>
#include <string>

#define kTileURL   "http://pepper.local:3333/gl/tiles/plain/%d-%d-%d.vector.pbf"
#define kSpriteURL "http://pepper.local:3333/gl/debug/img/sprite"

namespace llmr {

class Tile;

namespace platform {

// Restarts painting. This could for example trigger the event loop of the
// controlling application.
void restart(void *obj);

struct Response {
    int16_t code = -1;
    std::string body;
};

void request_http(std::string url, std::function<void(Response&)> func);
void request_http(std::string url, std::function<void(Response&)> func, std::function<void()> cb);

void async(std::function<void()> fn, std::function<void()> cb);

// Returns a relative timestamp in seconds. This value must be monotonic.
double time();

}

}

#endif
