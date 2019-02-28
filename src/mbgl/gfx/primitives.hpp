#pragma once

#include <cstddef>

namespace mbgl {
namespace gfx {

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

} // namespace gfx
} // namespace mbgl
