#pragma once

#include <mbgl/shader/attributes.hpp>
#include <mbgl/util/geometry.hpp>

#include <cmath>

namespace mbgl {

struct LineAttributes : gl::Attributes<
    attributes::a_pos,
    attributes::a_data<4>> {

    /*
     * @param p vertex position
     * @param e extrude normal
     * @param t texture normal
     * @param dir direction of the line cap (-1/0/1)
     */
    static Vertex vertex(Point<int16_t> p, Point<double> e, Point<bool> t, int8_t dir, int32_t linesofar = 0) {
        return Vertex {
            {
                static_cast<int16_t>((p.x * 2) | t.x),
                static_cast<int16_t>((p.y * 2) | t.y)
            },
            {
                // add 128 to store an byte in an unsigned byte
                static_cast<uint8_t>(::round(extrudeScale * e.x) + 128),
                static_cast<uint8_t>(::round(extrudeScale * e.y) + 128),

                // Encode the -1/0/1 direction value into the first two bits of .z of a_data.
                // Combine it with the lower 6 bits of `linesofar` (shifted by 2 bites to make
                // room for the direction value). The upper 8 bits of `linesofar` are placed in
                // the `w` component. `linesofar` is scaled down by `LINE_DISTANCE_SCALE` so that
                // we can store longer distances while sacrificing precision.

                // Encode the -1/0/1 direction value into .zw coordinates of a_data, which is normally covered
                // by linesofar, so we need to merge them.
                // The z component's first bit, as well as the sign bit is reserved for the direction,
                // so we need to shift the linesofar.
                static_cast<uint8_t>(((dir == 0 ? 0 : (dir < 0 ? -1 : 1 )) + 1) | ((linesofar & 0x3F) << 2)),
                static_cast<uint8_t>(linesofar >> 6)
            }
        };
    }

    /*
     * Scale the extrusion vector so that the normal length is this value.
     * Contains the "texture" normals (-1..1). This is distinct from the extrude
     * normals for line joins, because the x-value remains 0 for the texture
     * normal array, while the extrude normal actually moves the vertex to create
     * the acute/bevelled line join.
     */
    static const int8_t extrudeScale = 63;
};

using LineVertex = LineAttributes::Vertex;
static_assert(sizeof(LineVertex) == 8, "expected LineVertex size");

} // namespace mbgl
