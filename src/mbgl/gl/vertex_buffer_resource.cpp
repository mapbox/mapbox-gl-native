#include <mbgl/gl/context.hpp>
#include <mbgl/gl/vertex_buffer_resource.hpp>

namespace mbgl {
namespace gl {

VertexBufferResource::~VertexBufferResource() {
    auto& stats = buffer.get_deleter().context.renderingStats();
    stats.memVertexBuffers -= byteSize;
    assert(stats.memVertexBuffers >= 0);
}

} // namespace gl
} // namespace mbgl