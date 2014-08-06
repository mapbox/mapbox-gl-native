#ifndef MBGL_GEOMETRY_LINE_ATLAS
#define MBGL_GEOMETRY_LINE_ATLAS

#include <atomic>

namespace mbgl {

class LineAtlas {
public:
    LineAtlas(uint16_t width, uint16_t height);
    ~LineAtlas();

public:
    const uint32_t width = 0;
    const uint32_t height = 0;

private:
    std::atomic<bool> dirty;
};
};

#endif
