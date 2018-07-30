#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {
namespace gl {

void VertexArray::bind(Context& context, BufferID indexBuffer, const AttributeBindingArray& bindings) {
    context.bindVertexArray = state->vertexArray;
    state->indexBuffer = indexBuffer;

    state->bindings.reserve(bindings.size());
    for (AttributeLocation location = 0; location < bindings.size(); ++location) {
        if (state->bindings.size() <= location) {
            state->bindings.emplace_back(context, AttributeLocation(location));
        }
        state->bindings[location] = bindings[location];
    }
}

} // namespace gl
} // namespace mbgl
