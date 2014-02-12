#include <llmr/geometry/line_buffer.hpp>
#include <llmr/platform/gl.hpp>

#include <cmath>

using namespace llmr;

size_t LineVertexBuffer::add(vertex_type x, vertex_type y, float ex, float ey, int8_t tx, int8_t ty, int32_t linesofar) {
    size_t idx = index();
    void *data = addElement();

    int16_t *coords = static_cast<int16_t *>(data);
    coords[0] = (x * 2) | tx;
    coords[1] = (y * 2) | ty;
    coords[2] = linesofar;

    int8_t *extrude = static_cast<int8_t *>(data);
    extrude[6] = round(extrudeScale * ex);
    extrude[7] = round(extrudeScale * ey);

    return idx;
}
