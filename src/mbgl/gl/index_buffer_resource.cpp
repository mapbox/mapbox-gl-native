#include <mbgl/gl/context.hpp>
#include <mbgl/gl/index_buffer_resource.hpp>

namespace mbgl {
namespace gl {

IndexBufferResource::~IndexBufferResource() {
    auto& stats = buffer.get_deleter().context.renderingStats();
    stats.memIndexBuffers -= byteSize;
    assert(stats.memIndexBuffers >= 0);
}

} // namespace gl
} // namespace mbgl