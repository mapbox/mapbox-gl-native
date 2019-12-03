#pragma once

#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/gl/object.hpp>

namespace mbgl {
namespace gl {

class IndexBufferResource : public gfx::IndexBufferResource {
public:
    IndexBufferResource(UniqueBuffer&& buffer_, int byteSize_) : buffer(std::move(buffer_)), byteSize(byteSize_) {}
    ~IndexBufferResource();

    UniqueBuffer buffer;
    int byteSize;
};

} // namespace gl
} // namespace mbgl
