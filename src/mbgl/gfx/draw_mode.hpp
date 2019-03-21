#pragma once

#include <mbgl/gfx/types.hpp>

#include <cassert>

namespace mbgl {
namespace gfx {

class DrawMode {
protected:
    DrawMode(DrawModeType type_, float value_)
        : type(type_), value(value_) {
    }

public:
    const DrawModeType type;
    const float value;
};

class Points : public DrawMode {
public:
    explicit Points(float pointSize_) : DrawMode(DrawModeType::Points, pointSize_), pointSize(pointSize_) {
        assert(size > 0);
    }

    static constexpr PrimitiveType primitive = PrimitiveType::Point;
    static constexpr std::size_t bufferGroupSize = 1;
    float pointSize;
};

class Lines : public DrawMode {
public:
    explicit Lines(float lineWidth_) : DrawMode(DrawModeType::Lines, lineWidth_), lineWidth(lineWidth_) {
        assert(size > 0);
    }

    static constexpr PrimitiveType primitive = PrimitiveType::Line;
    static constexpr std::size_t bufferGroupSize = 2;
    float lineWidth;
};

class LineStrip : public DrawMode {
public:
    explicit LineStrip(float lineWidth_) : DrawMode(DrawModeType::LineStrip, lineWidth_), lineWidth(lineWidth_) {
        assert(size > 0);
    }

    // LineStrip is a form of "Line" rendering, but the element buffer
    // cannot be grouped into logical elements beyond a single Point.
    static constexpr PrimitiveType primitive = PrimitiveType::Line;
    static constexpr std::size_t bufferGroupSize = 1;
    float lineWidth;
};

class Triangles : public DrawMode {
public:
    explicit Triangles() : DrawMode(DrawModeType::Triangles, 0) {
    }

    static constexpr PrimitiveType primitive = PrimitiveType::Triangle;
    static constexpr std::size_t bufferGroupSize = 3;
};

class TriangleStrip : public DrawMode {
public:
    explicit TriangleStrip() : DrawMode(DrawModeType::TriangleStrip, 0) {
    }

    // TriangleStrip is a form of "Triangle" rendering, but the element buffer
    // cannot be grouped into logical elements beyond a single Point.
    static constexpr PrimitiveType primitive = PrimitiveType::Triangle;
    static constexpr std::size_t bufferGroupSize = 1;
};

} // namespace gfx
} // namespace mbgl
