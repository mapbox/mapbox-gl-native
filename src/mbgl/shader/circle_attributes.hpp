#pragma once

#include <mbgl/shader/attributes.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

struct CircleAttributes : gl::Attributes<
    attributes::a_pos>
{
    /*
     * @param {number} x vertex position
     * @param {number} y vertex position
     * @param {number} ex extrude normal
     * @param {number} ey extrude normal
     */
    static Vertex vertex(Point<int16_t> p, float ex, float ey) {
        return Vertex {
            {
                static_cast<int16_t>((p.x * 2) + ((ex + 1) / 2)),
                static_cast<int16_t>((p.y * 2) + ((ey + 1) / 2))
            }
        };
    }
};

using CircleVertex = CircleAttributes::Vertex;
static_assert(sizeof(CircleVertex) == 4, "expected CircleVertex size");

} // namespace mbgl
