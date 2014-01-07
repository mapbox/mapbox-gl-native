#ifndef LLMR_MAP_MAP
#define LLMR_MAP_MAP

#include "../platform/platform.hpp"
#include "../renderer/painter.hpp"
#include "transform.hpp"

namespace llmr {

class tile;

class map {
public:
    map(class platform *platform, class painter *painter);
    ~map();

    /* callback */
    bool render();
    void moveBy(double dx, double dy);
    void scaleBy(double ds, double cx, double cy);
    void tileLoaded(tile *tile);
    void tileFailed(tile *tile);

private:
    platform *platform;
    painter *painter;
    transform *transform;

    std::vector<tile *> tiles;
};

}

#endif
