#pragma once

#include <mbgl/gl/attribute.hpp>

#include <array>
#include <cstdint>
#include <cmath>

namespace mbgl {

class CollisionBoxVertex {
public:
    CollisionBoxVertex(int16_t x, int16_t y, float ox, float oy, float maxzoom, float placementZoom)
        : a_pos { x, y },
          a_extrude {
              static_cast<int16_t>(::round(ox)),
              static_cast<int16_t>(::round(oy))
          },
          a_data {
              static_cast<uint8_t>(maxzoom * 10),
              static_cast<uint8_t>(placementZoom * 10)
          } {}

    const int16_t a_pos[2];
    const int16_t a_extrude[2];
    const uint8_t a_data[2];
};

namespace gl {

template <class Shader>
struct AttributeBindings<Shader, CollisionBoxVertex> {
    std::array<AttributeBinding, 3> operator()(const Shader& shader) {
        return {{
            MBGL_MAKE_ATTRIBUTE_BINDING(CollisionBoxVertex, shader, a_pos),
            MBGL_MAKE_ATTRIBUTE_BINDING(CollisionBoxVertex, shader, a_extrude),
            MBGL_MAKE_ATTRIBUTE_BINDING(CollisionBoxVertex, shader, a_data)
        }};
    };
};

} // namespace gl
} // namespace mbgl
