#pragma once

#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gl/object.hpp>

namespace mbgl {
namespace gl {

class VertexBufferResource : public gfx::VertexBufferResource {
public:
    VertexBufferResource(UniqueBuffer&& buffer_) : buffer(std::move(buffer_)) {
    }

    UniqueBuffer buffer;
};

} // namespace gl
} // namespace mbgl
