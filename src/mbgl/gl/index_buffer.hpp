#pragma once

#include <mbgl/gl/object.hpp>

namespace mbgl {
namespace gl {

template <class DrawMode>
class IndexBuffer {
public:
    std::size_t indexCount;
    UniqueBuffer buffer;
};

} // namespace gl
} // namespace mbgl
