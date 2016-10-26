#pragma once

#include <mbgl/gl/attribute.hpp>

#include <array>
#include <cstdint>

namespace mbgl {

class CircleVertex {
public:
    /*
     * @param {number} x vertex position
     * @param {number} y vertex position
     * @param {number} ex extrude normal
     * @param {number} ey extrude normal
     */
    CircleVertex(int16_t x, int16_t y, float ex, float ey)
        : a_pos {
            static_cast<int16_t>((x * 2) + ((ex + 1) / 2)),
            static_cast<int16_t>((y * 2) + ((ey + 1) / 2))
        } {}

    const int16_t a_pos[2];
};

namespace gl {

template <class Shader>
struct AttributeBindings<Shader, CircleVertex> {
    std::array<AttributeBinding, 1> operator()(const Shader& shader) {
        return {{
            MBGL_MAKE_ATTRIBUTE_BINDING(CircleVertex, shader, a_pos)
        }};
    };
};

} // namespace gl
} // namespace mbgl
