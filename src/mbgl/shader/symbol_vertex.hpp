#pragma once

#include <mbgl/gl/attribute.hpp>

#include <array>
#include <cstdint>
#include <cmath>

namespace mbgl {

class SymbolVertex {
public:
    SymbolVertex(int16_t x, int16_t y, float ox, float oy, uint16_t tx, uint16_t ty, float minzoom, float maxzoom, float labelminzoom, uint8_t labelangle)
        : a_pos {
              x,
              y
          },
          a_offset {
              static_cast<int16_t>(::round(ox * 64)),  // use 1/64 pixels for placement
              static_cast<int16_t>(::round(oy * 64))
          },
          a_texture_pos {
              static_cast<uint16_t>(tx / 4),
              static_cast<uint16_t>(ty / 4)
          },
          a_data {
              static_cast<uint8_t>(labelminzoom * 10), // 1/10 zoom levels: z16 == 160
              static_cast<uint8_t>(labelangle),
              static_cast<uint8_t>(minzoom * 10),
              static_cast<uint8_t>(::fmin(maxzoom, 25) * 10)
          } {}

    const int16_t a_pos[2];
    const int16_t a_offset[2];
    const uint16_t a_texture_pos[2];
    const uint8_t a_data[4];
};

namespace gl {

template <class Shader>
struct AttributeBindings<Shader, SymbolVertex> {
    std::array<AttributeBinding, 4> operator()(const Shader& shader) {
        return {{
            MBGL_MAKE_ATTRIBUTE_BINDING(SymbolVertex, shader, a_pos),
            MBGL_MAKE_ATTRIBUTE_BINDING(SymbolVertex, shader, a_offset),
            MBGL_MAKE_ATTRIBUTE_BINDING(SymbolVertex, shader, a_texture_pos),
            MBGL_MAKE_ATTRIBUTE_BINDING(SymbolVertex, shader, a_data)
        }};
    };
};

} // namespace gl
} // namespace mbgl
