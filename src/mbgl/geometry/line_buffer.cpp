#include <mbgl/geometry/line_buffer.hpp>
#include <mbgl/gl/gl.hpp>

#include <cmath>

namespace mbgl {

GLsizei LineVertexBuffer::add(vertex_type x, vertex_type y, float ex, float ey, bool tx, bool ty, int8_t dir, int32_t linesofar) {
    GLsizei idx = index();
    void *data = addElement();

    int16_t *coords = static_cast<int16_t *>(data);
    coords[0] = (x * 2) | tx;
    coords[1] = (y * 2) | ty;

    uint8_t *ubytes = static_cast<uint8_t *>(data);
    // add 128 to store an byte in an unsigned byte
    ubytes[4] = ::round(extrudeScale * ex) + 128;
    ubytes[5] = ::round(extrudeScale * ey) + 128;

    // Encode the -1/0/1 direction value into the first two bits of .z of a_data.
    // Combine it with the lower 6 bits of `linesofar` (shifted by 2 bites to make
    // room for the direction value). The upper 8 bits of `linesofar` are placed in
    // the `w` component. `linesofar` is scaled down by `LINE_DISTANCE_SCALE` so that
    // we can store longer distances while sacrificing precision.

    // Encode the -1/0/1 direction value into .zw coordinates of a_data, which is normally covered
    // by linesofar, so we need to merge them.
    // The z component's first bit, as well as the sign bit is reserved for the direction,
    // so we need to shift the linesofar.

    ubytes[6] = ((dir == 0 ? 0 : (dir < 0 ? -1 : 1 )) + 1) | ((linesofar & 0x3F) << 2);
    ubytes[7] = linesofar >> 6;

    return idx;
}

} // namespace mbgl
