#pragma once

#include <mbgl/util/variant.hpp>

namespace mbgl {
namespace gl {

class Points {
public:
    float pointSize;
};

class Lines {
public:
    float lineWidth;
};

class LineStrip {
public:
    float lineWidth;
};

class Triangles {};
class TriangleStrip {};

using DrawMode = variant<
    Points,
    Lines,
    LineStrip,
    Triangles,
    TriangleStrip>;

} // namespace gl
} // namespace mbgl
