#ifndef LLMR_MAP_MAP
#define LLMR_MAP_MAP

#include "../platform/platform.hpp"
#include "../renderer/painter.hpp"
#include "transform.hpp"

namespace llmr {

class tile;

class map {
public:
    map(class platform *platform);
    ~map();

    void setup();
    void resize(uint32_t width, uint32_t height);

    /* callback */
    bool render();
    void moveBy(double dx, double dy);
    void scaleBy(double ds, double cx, double cy);
    void tileLoaded(tile *tile);
    void tileFailed(tile *tile);

private:
    platform *platform;
    transform *transform;
    painter *painter;

    std::vector<tile *> tiles;
};

}

#endif
