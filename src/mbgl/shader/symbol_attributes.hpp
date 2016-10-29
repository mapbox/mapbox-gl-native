#pragma once

#include <mbgl/shader/attributes.hpp>
#include <mbgl/util/geometry.hpp>

#include <cmath>

namespace mbgl {

struct SymbolAttributes : gl::Attributes<
    attributes::a_pos,
    attributes::a_offset,
    attributes::a_texture_pos,
    attributes::a_data<4>>
{
    static Vertex vertex(Point<float> a,
                         Point<float> o,
                         uint16_t tx,
                         uint16_t ty,
                         float minzoom,
                         float maxzoom,
                         float labelminzoom,
                         uint8_t labelangle) {
        return Vertex {
            {
                static_cast<int16_t>(a.x),
                static_cast<int16_t>(a.y)
            },
            {
                static_cast<int16_t>(::round(o.x * 64)),  // use 1/64 pixels for placement
                static_cast<int16_t>(::round(o.y * 64))
            },
            {
                static_cast<uint16_t>(tx / 4),
                static_cast<uint16_t>(ty / 4)
            },
            {
                static_cast<uint8_t>(labelminzoom * 10), // 1/10 zoom levels: z16 == 160
                static_cast<uint8_t>(labelangle),
                static_cast<uint8_t>(minzoom * 10),
                static_cast<uint8_t>(::fmin(maxzoom, 25) * 10)
            }
        };
    }
};

using SymbolVertex = SymbolAttributes::Vertex;
static_assert(sizeof(SymbolVertex) == 16, "expected SymbolVertex size");

} // namespace mbgl
