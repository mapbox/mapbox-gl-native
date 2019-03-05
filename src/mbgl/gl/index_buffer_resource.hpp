#pragma once

#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/gl/object.hpp>

namespace mbgl {
namespace gl {

class IndexBufferResource : public gfx::IndexBufferResource {
public:
    IndexBufferResource(UniqueBuffer&& buffer_) : buffer(std::move(buffer_)) {
    }

    UniqueBuffer buffer;
};

} // namespace gl
} // namespace mbgl
