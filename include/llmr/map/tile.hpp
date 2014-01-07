#ifndef LLMR_MAP_TILE
#define LLMR_MAP_TILE

#include "../geometry/linevertexbuffer.hpp"

#include <stdint.h>
#include <vector>

namespace llmr {

class tile {
public:
    tile(int32_t z, int32_t x, int32_t y);

    void setData(uint8_t *data, uint32_t bytes);
    bool parse();
    void parseLayer(const uint8_t *data, uint32_t bytes);
    void parseFeature(const uint8_t *data, uint32_t bytes);
    void loadGeometry(const uint8_t *data, uint32_t bytes);

public:
    const int32_t z, x, y;
    bool loaded;
    linevertexbuffer lineVertex;

private:
    // Source data
    uint8_t *data;
    uint32_t bytes;

};

}

#endif
