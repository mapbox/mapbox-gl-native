#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/gl/index_buffer_resource.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {
namespace gl {

void VertexArray::bind(Context& context,
                       const gfx::IndexBuffer& indexBuffer,
                       const AttributeBindingArray& bindings) {
    context.bindVertexArray = state->vertexArray;
    state->indexBuffer = indexBuffer.getResource<gl::IndexBufferResource>().buffer;

    state->bindings.reserve(bindings.size());

    // NOLINTNEXTLINE(bugprone-too-small-loop-variable)
    for (AttributeLocation location = 0; location < bindings.size(); ++location) {
        if (state->bindings.size() <= location) {
            state->bindings.emplace_back(context, AttributeLocation(location));
        }
        state->bindings[location] = bindings[location];
    }
}

} // namespace gl
} // namespace mbgl
