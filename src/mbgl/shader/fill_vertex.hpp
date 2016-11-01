#pragma once

#include <mbgl/gl/attribute.hpp>

#include <vector>
#include <cstdint>

namespace mbgl {

class FillVertex {
public:
    FillVertex(int16_t x, int16_t y)
        : a_pos { x, y } {}

    const int16_t a_pos[2];
};

namespace gl {

template <class Shader>
struct AttributeBindings<Shader, FillVertex> {
    std::vector<AttributeBinding> operator()(const Shader& shader) {
        return {{
            MBGL_MAKE_ATTRIBUTE_BINDING(FillVertex, shader, a_pos)
        }};
    };
};

} // namespace gl
} // namespace mbgl
