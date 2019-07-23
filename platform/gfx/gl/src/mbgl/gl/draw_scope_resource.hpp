#pragma once

#include <mbgl/gfx/draw_scope.hpp>
#include <mbgl/gl/vertex_array.hpp>

namespace mbgl {
namespace gl {

class DrawScopeResource : public gfx::DrawScopeResource {
public:
    DrawScopeResource(VertexArray&& vertexArray_) : vertexArray(std::move(vertexArray_)) {
    }

    VertexArray vertexArray;
};

} // namespace gl
} // namespace mbgl
