#ifndef LLMR_PLATFORM_IOS
#define LLMR_PLATFORM_IOS

#include <memory>

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

// Returns a relative timestamp in seconds. This value must be monotonic.
double time();

}

}

#endif
