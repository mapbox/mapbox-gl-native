#pragma once

#include <mbgl/gfx/types.hpp>

#include <cassert>

namespace mbgl {
namespace gfx {

class DrawMode {
protected:
    DrawMode(DrawModeType type_, float size_)
        : type(type_), size(size_) {
    }

public:
    const DrawModeType type;
    const float size;
};

template <class> struct BufferGroupSizeOf;
template <class> struct PrimitiveTypeOf;

class Points : public DrawMode {
public:
    explicit Points(float pointSize_) : DrawMode(DrawModeType::Points, pointSize_) {
        assert(size > 0);
    }
};

template <> struct BufferGroupSizeOf<Points> : std::integral_constant<std::size_t, 1> {};
template <> struct PrimitiveTypeOf<Points> : std::integral_constant<PrimitiveType, PrimitiveType::Point> {};

class Lines : public DrawMode {
public:
    explicit Lines(float lineWidth_) : DrawMode(DrawModeType::Lines, lineWidth_) {
        assert(size > 0);
    }
};

template <> struct BufferGroupSizeOf<Lines> : std::integral_constant<std::size_t, 2> {};
template <> struct PrimitiveTypeOf<Lines> : std::integral_constant<PrimitiveType, PrimitiveType::Line> {};

// LineStrip is a form of "Line" rendering, but the element buffer
// cannot be grouped into logical elements beyond a single Point.
class LineStrip : public DrawMode {
public:
    explicit LineStrip(float lineWidth_) : DrawMode(DrawModeType::LineStrip, lineWidth_) {
        assert(size > 0);
    }
};

template <> struct BufferGroupSizeOf<LineStrip> : std::integral_constant<std::size_t, 1> {};
template <> struct PrimitiveTypeOf<LineStrip> : std::integral_constant<PrimitiveType, PrimitiveType::Line> {};

class Triangles : public DrawMode {
public:
    explicit Triangles() : DrawMode(DrawModeType::Triangles, 0) {
    }
};

template <> struct BufferGroupSizeOf<Triangles> : std::integral_constant<std::size_t, 3> {};
template <> struct PrimitiveTypeOf<Triangles> : std::integral_constant<PrimitiveType, PrimitiveType::Triangle> {};

// TriangleStrip is a form of "Triangle" rendering, but the element buffer
// cannot be grouped into logical elements beyond a single Point.
class TriangleStrip : public DrawMode {
public:
    explicit TriangleStrip() : DrawMode(DrawModeType::TriangleStrip, 0) {
    }
};

template <> struct BufferGroupSizeOf<TriangleStrip> : std::integral_constant<std::size_t, 1> {};
template <> struct PrimitiveTypeOf<TriangleStrip> : std::integral_constant<PrimitiveType, PrimitiveType::Triangle> {};

} // namespace gfx
} // namespace mbgl
