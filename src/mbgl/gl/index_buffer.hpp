#pragma once

#include <mbgl/gl/object.hpp>

namespace mbgl {
namespace gl {

class IndexBuffer {
public:
    std::size_t elements;
    UniqueBuffer buffer;
};

} // namespace gl
} // namespace mbgl
