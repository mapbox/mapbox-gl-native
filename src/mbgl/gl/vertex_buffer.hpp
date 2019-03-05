#pragma once

#include <mbgl/gl/object.hpp>

namespace mbgl {
namespace gl {

template <class V>
class VertexBuffer {
public:
    std::size_t elements;
    UniqueBuffer buffer;
};

} // namespace gl
} // namespace mbgl
