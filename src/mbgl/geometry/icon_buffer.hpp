#ifndef MBGL_GEOMETRY_ICON_BUFFER
#define MBGL_GEOMETRY_ICON_BUFFER

#include <mbgl/geometry/buffer.hpp>

#include <array>

namespace mbgl {

    class IconVertexBuffer : public Buffer<
    16
    > {
    public:
        size_t add(int16_t x, int16_t y, float ox, float oy, int16_t tx, int16_t ty, float minzoom, float maxzoom, float labelminzoom);

    };

}

#endif
