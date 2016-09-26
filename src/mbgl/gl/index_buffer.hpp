#pragma once

#include <mbgl/gl/object.hpp>

namespace mbgl {
namespace gl {

class Line {
public:
    Line(uint16_t a_, uint16_t b_)
        : a(a_), b(b_) {}

    uint16_t a;
    uint16_t b;

    static constexpr std::size_t IndexCount = 2;
};

class Triangle {
public:
    Triangle(uint16_t a_, uint16_t b_, uint16_t c_)
        : a(a_), b(b_), c(c_) {}

    uint16_t a;
    uint16_t b;
    uint16_t c;

    static constexpr std::size_t IndexCount = 3;
};

template <class Primitive>
class IndexBuffer {
public:
    static_assert(std::is_same<Primitive, Line>::value || std::is_same<Primitive, Triangle>::value,
                  "primitive must be Line or Triangle");
    static constexpr std::size_t primitiveSize = sizeof(Primitive);
    UniqueBuffer buffer;
};

} // namespace gl
} // namespace mbgl
