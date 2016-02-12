#include <mbgl/geometry/line_buffer.hpp>
#include <mbgl/gl/gl.hpp>

#include <cmath>

using namespace mbgl;

GLsizei LineVertexBuffer::add(vertex_type x, vertex_type y, float ex, float ey, bool tx, bool ty, int8_t dir, int32_t linesofar) {
    GLsizei idx = index();
    void *data = addElement();

    int16_t *coords = static_cast<int16_t *>(data);
    coords[0] = (x * 2) | tx;
    coords[1] = (y * 2) | ty;

    int8_t *extrude = static_cast<int8_t *>(data);
    extrude[4] = ::round(extrudeScale * ex);
    extrude[5] = ::round(extrudeScale * ey);

    // Encode the -1/0/1 direction value into .zw coordinates of a_data, which is normally covered
    // by linesofar, so we need to merge them.
    // The z component's first bit, as well as the sign bit is reserved for the direction,
    // so we need to shift the linesofar.
    extrude[6] = ((dir < 0) ? -1 : 1) * ((dir ? 1 : 0) | static_cast<int8_t>((linesofar << 1) & 0x7F));
    extrude[7] = ((linesofar >> 6) & 0xFF) - 128;

    return idx;
}
