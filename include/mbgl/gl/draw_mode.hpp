#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/primitives.hpp>

#include <cassert>

namespace mbgl {
namespace gl {

class Points {
public:
    using Primitive = Point;

    static constexpr std::size_t bufferGroupSize = 1;
    static constexpr PrimitiveType primitiveType = PrimitiveType::Points;

    explicit Points(float pointSize_) : pointSize(pointSize_) {}

    float pointSize;
};

class Lines {
public:
    using Primitive = Line;

    static constexpr std::size_t bufferGroupSize = 2;
    static constexpr PrimitiveType primitiveType = PrimitiveType::Lines;

    explicit Lines(float lineWidth_) : lineWidth(lineWidth_) {
        assert(lineWidth > 0);
    }

    float lineWidth;
};

class LineStrip {
public:
    // LineStrip is a form of "Line" rendering, but the element buffer
    // cannot be grouped into logical elements beyond a single Point.
    using Primitive = Line;

    static constexpr std::size_t bufferGroupSize = 1;
    static constexpr PrimitiveType primitiveType = PrimitiveType::LineStrip;

    explicit LineStrip(float lineWidth_) : lineWidth(lineWidth_) {
        assert(lineWidth > 0);
    }

    float lineWidth;
};

class Triangles {
public:
    using Primitive = Triangle;

    static constexpr std::size_t bufferGroupSize = 3;
    static constexpr PrimitiveType primitiveType = PrimitiveType::Triangles;
};

class TriangleStrip {
public:
    // TriangleStrip is a form of "Triangle" rendering, but the element buffer
    // cannot be grouped into logical elements beyond a single Point.
    using Primitive = Triangle;

    static constexpr std::size_t bufferGroupSize = 1;
    static constexpr PrimitiveType primitiveType = PrimitiveType::TriangleStrip;
};

// Special draw mode for use with VertexVector<Indexed, Vertex>, in which
// case the true draw mode is denoted by the IndexVector type.
class Indexed {
public:
    static constexpr std::size_t bufferGroupSize = 1;
};

} // namespace gl
} // namespace mbgl
