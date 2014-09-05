#include <mbgl/geometry/text_buffer.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>

using namespace mbgl;

const double TextVertexBuffer::angleFactor = 128.0 / M_PI;

size_t TextVertexBuffer::add(int16_t x, int16_t y, float ox, float oy, uint16_t tx, uint16_t ty, float angle, float minzoom, std::array<float, 2> range, float maxzoom, float labelminzoom) {
    size_t idx = index();
    void *data = addElement();

    int16_t *shorts = static_cast<int16_t *>(data);
    shorts[0] = x;
    shorts[1] = y;
    shorts[2] = std::round(ox * 64); // use 1/64 pixels for placement
    shorts[3] = std::round(oy * 64);

    uint8_t *ubytes = static_cast<uint8_t *>(data);
    ubytes[8] = tx / 4;
    ubytes[9] = ty / 4;
    ubytes[10] = labelminzoom * 10;
    ubytes[11] = minzoom * 10; // 1/10 zoom levels: z16 == 160.
    ubytes[12] = fmin(maxzoom, 25) * 10; // 1/10 zoom levels: z16 == 160.
    ubytes[13] = (int16_t)round(angle * angleFactor) % 256;
    ubytes[14] = util::max((int16_t)std::round(range[0] * angleFactor), (int16_t)0) % 256;
    ubytes[15] = util::min((int16_t)std::round(range[1] * angleFactor), (int16_t)255) % 256;

    return idx;
}
