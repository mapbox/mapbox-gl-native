#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>

namespace mbgl {

size_t TextVertexBuffer::add(int16_t x, int16_t y, float ox, float oy, uint16_t tx, uint16_t ty, float minzoom, float maxzoom, float labelminzoom, uint8_t labelangle) {
    const size_t idx = index();
    void *data = addElement();

    int16_t *shorts = static_cast<int16_t *>(data);
    shorts[0] /* pos */ = x;
    shorts[1] /* pos */ = y;
    shorts[2] /* offset */ = ::round(ox * 64); // use 1/64 pixels for placement
    shorts[3] /* offset */ = ::round(oy * 64);

    uint16_t *ushorts = static_cast<uint16_t *>(data);
    // a_texture_pos
    ushorts[4] /* tex */ = tx / 4;
    ushorts[5] /* tex */ = ty / 4;

    uint8_t *ubytes = static_cast<uint8_t *>(data);
    // a_data
    ubytes[12] /* labelminzoom */ = labelminzoom * 10;
    ubytes[13] /* labelangle */ = labelangle;

    ubytes[14] /* minzoom */ = minzoom * 10; // 1/10 zoom levels: z16 == 160.
    ubytes[15] /* maxzoom */ = ::fmin(maxzoom, 25) * 10; // 1/10 zoom levels: z16 == 160.

    return idx;
}

} // namespace mbgl

