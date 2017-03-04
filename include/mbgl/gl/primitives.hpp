#pragma once

namespace mbgl {
namespace gl {

class Point {
public:
    static constexpr std::size_t vertexCount = 1;
};

class Line {
public:
    static constexpr std::size_t vertexCount = 2;
};

class Triangle {
public:
    static constexpr std::size_t vertexCount = 3;
};

} // namespace gl
} // namespace mbgl
