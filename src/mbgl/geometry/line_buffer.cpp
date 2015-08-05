#include <mbgl/geometry/line_buffer.hpp>
#include <mbgl/platform/gl.hpp>

#include <cmath>

using namespace mbgl;

size_t LineVertexBuffer::add(vertex_type x, vertex_type y, float ex, float ey, int8_t tx, int8_t ty, int32_t linesofar) {
    size_t idx = index();
    void *data = addElement();

    int16_t *coords = static_cast<int16_t *>(data);
    coords[0] = (x * 2) | tx;
    coords[1] = (y * 2) | ty;

    int8_t *extrude = static_cast<int8_t *>(data);
    extrude[4] = ::round(extrudeScale * ex);
    extrude[5] = ::round(extrudeScale * ey);
    extrude[6] = static_cast<int8_t>(linesofar / 128);
    extrude[7] = static_cast<int8_t>(linesofar % 128);

    return idx;
}
