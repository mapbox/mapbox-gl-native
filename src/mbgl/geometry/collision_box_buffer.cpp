#include <mbgl/geometry/collision_box_buffer.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>

namespace mbgl {

size_t CollisionBoxVertexBuffer::add(int16_t x, int16_t y, float ox, float oy, float maxzoom, float placementZoom) {
    const size_t idx = index();
    void *data = addElement();

    int16_t *shorts = static_cast<int16_t *>(data);
    shorts[0] /* pos */ = x;
    shorts[1] /* pos */ = y;
    shorts[2] /* offset */ = ::round(ox); // use 1/64 pixels for placement
    shorts[3] /* offset */ = ::round(oy);

    uint8_t *ubytes = static_cast<uint8_t *>(data);
    // a_data1
    ubytes[8] = maxzoom * 10;
    ubytes[9] = placementZoom * 10;

    return idx;
}

}
