#ifndef LLMR_PLATFORM_IOS
#define LLMR_PLATFORM_IOS

#include <memory>

namespace llmr {

class Tile;

namespace platform {

void restart(void *obj);
void request(void *obj, std::shared_ptr<Tile> tile);
double time();

}

}

#endif
