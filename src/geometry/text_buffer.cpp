#include <llmr/geometry/text_buffer.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/util/math.hpp>

#include <cmath>

using namespace llmr;

const double TextVertexBuffer::angleFactor = 128 / M_PI;


size_t TextVertexBuffer::add(const GlyphPosition& pos) {
    size_t idx = index();
    void *data = addElement();

    int16_t *shorts = static_cast<int16_t *>(data);
    shorts[0] = pos.x;
    shorts[1] = pos.y;
    shorts[2] = round(pos.ox * 64); // use 1/64 pixels for placement
    shorts[3] = round(pos.oy * 64);

    uint8_t *ubytes = static_cast<uint8_t *>(data);
    ubytes[8] = pos.tx / 4;
    ubytes[9] = pos.ty / 4;
    ubytes[10] = pos.labelminzoom * 10;
    ubytes[11] = pos.minzoom * 10; // 1/10 zoom levels: z16 == 160.
    ubytes[12] = fmin(pos.maxzoom, 25) * 10; // 1/10 zoom levels: z16 == 160.
    ubytes[13] = (int16_t)round(pos.angle * angleFactor) % 256;
    ubytes[14] = util::max((int16_t)round(pos.rangefrom * angleFactor), (int16_t)0) % 256;
    ubytes[15] = util::min((int16_t)round(pos.rangeto * angleFactor), (int16_t)255) % 256;

    return idx;
}
