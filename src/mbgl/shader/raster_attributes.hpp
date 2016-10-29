#pragma once

#include <mbgl/shader/attributes.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

struct RasterAttributes : gl::Attributes<
    attributes::a_pos,
    attributes::a_texture_pos>
{
    static Vertex vertex(Point<int16_t> p, Point<uint16_t> t) {
        return Vertex {
            {
                p.x,
                p.y
            },
            {
                t.x,
                t.y
            }
        };
    }
};

using RasterVertex = RasterAttributes::Vertex;
static_assert(sizeof(RasterVertex) == 8, "expected RasterVertex size");

} // namespace mbgl
