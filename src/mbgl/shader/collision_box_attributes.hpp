#pragma once

#include <mbgl/shader/attributes.hpp>
#include <mbgl/util/geometry.hpp>

#include <cmath>

namespace mbgl {

struct CollisionBoxAttributes : gl::Attributes<
    attributes::a_pos,
    attributes::a_extrude,
    attributes::a_data<2>>
{
    static Vertex vertex(Point<float> a, Point<float> o, float maxzoom, float placementZoom) {
        return Vertex {
            {
                static_cast<int16_t>(a.x),
                static_cast<int16_t>(a.y)
            },
            {
                static_cast<int16_t>(::round(o.x)),
                static_cast<int16_t>(::round(o.y))
            },
            {
                static_cast<uint8_t>(maxzoom * 10),
                static_cast<uint8_t>(placementZoom * 10)
            }
        };
    }
};

using CollisionBoxVertex = CollisionBoxAttributes::Vertex;
static_assert(sizeof(CollisionBoxVertex) == 10, "expected CollisionBoxVertex size");

} // namespace mbgl
