#pragma once

#include <mbgl/gl/attribute.hpp>

#include <array>
#include <cstdint>

namespace mbgl {

class RasterVertex {
public:
    RasterVertex(int16_t x, int16_t y, uint16_t tx, uint16_t ty)
        : a_pos {
              x,
              y
          },
          a_texture_pos {
              tx,
              ty
          } {}

    const int16_t a_pos[2];
    const uint16_t a_texture_pos[2];
};

namespace gl {

template <class Shader>
struct AttributeBindings<Shader, RasterVertex> {
    std::array<AttributeBinding, 2> operator()(const Shader& shader) {
        return {{
            MBGL_MAKE_ATTRIBUTE_BINDING(RasterVertex, shader, a_pos),
            MBGL_MAKE_ATTRIBUTE_BINDING(RasterVertex, shader, a_texture_pos)
        }};
    };
};

} // namespace gl
} // namespace mbgl
