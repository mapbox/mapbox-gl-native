#pragma once

#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gl/object.hpp>

namespace mbgl {
namespace gl {

class VertexBufferResource : public gfx::VertexBufferResource {
public:
    VertexBufferResource(UniqueBuffer&& buffer_, int byteSize_) : buffer(std::move(buffer_)), byteSize(byteSize_) {}
    ~VertexBufferResource();

    UniqueBuffer buffer;
    int byteSize;
};

} // namespace gl
} // namespace mbgl
