#include <mbgl/geometry/icon_buffer.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>

namespace mbgl {

size_t IconVertexBuffer::add(int16_t x, int16_t y, float ox, float oy, int16_t tx, int16_t ty, float minzoom, float maxzoom, float labelminzoom) {
    const size_t idx = index();
    void *data = addElement();

    int16_t *shorts = static_cast<int16_t *>(data);
    shorts[0] /* pos */ = x;
    shorts[1] /* pos */ = y;
    shorts[2] /* offset */ = ::round(ox * 64); // use 1/64 pixels for placement
    shorts[3] /* offset */ = ::round(oy * 64);

    uint8_t *ubytes = static_cast<uint8_t *>(data);
    // a_data1
    ubytes[8] /* tex */ = tx / 4;
    ubytes[9] /* tex */ = ty / 4;
    ubytes[10] /* labelminzoom */ = labelminzoom * 10;

    // a_data2
    ubytes[12] /* minzoom */ = minzoom * 10; // 1/10 zoom levels: z16 == 160.
    ubytes[13] /* maxzoom */ = ::fmin(maxzoom, 25) * 10; // 1/10 zoom levels: z16 == 160.

    return idx;
}

}
