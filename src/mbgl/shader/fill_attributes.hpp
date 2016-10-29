#pragma once

#include <mbgl/shader/attributes.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

struct FillAttributes : gl::Attributes<
    attributes::a_pos> {

    static Vertex vertex(Point<int16_t> p) {
        return Vertex {
            {
                p.x,
                p.y
            }
        };
    }
};

using FillVertex = FillAttributes::Vertex;
static_assert(sizeof(FillVertex) == 4, "expected FillVertex size");

} // namespace mbgl
