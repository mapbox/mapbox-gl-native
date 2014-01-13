#ifndef LLMR_PLATFORM_IOS
#define LLMR_PLATFORM_IOS

namespace llmr {

class tile;

namespace platform {

void restart(void *obj);
void request(void *obj, tile *tile);

}

}

#endif
