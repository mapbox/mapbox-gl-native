#pragma once

#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {
namespace gl {
namespace extension {

class VertexArray {
public:
    template <typename Fn>
    VertexArray(const Fn& loadExtension)
        : bindVertexArray(
              loadExtension({ { "GL_ARB_vertex_array_object", "glBindVertexArray" },
                              { "GL_OES_vertex_array_object", "glBindVertexArrayOES" },
                              { "GL_APPLE_vertex_array_object", "glBindVertexArrayAPPLE" } })),
          deleteVertexArrays(
              loadExtension({ { "GL_ARB_vertex_array_object", "glDeleteVertexArrays" },
                              { "GL_OES_vertex_array_object", "glDeleteVertexArraysOES" },
                              { "GL_APPLE_vertex_array_object", "glDeleteVertexArraysAPPLE" } })),
          genVertexArrays(
              loadExtension({ { "GL_ARB_vertex_array_object", "glGenVertexArrays" },
                              { "GL_OES_vertex_array_object", "glGenVertexArraysOES" },
                              { "GL_APPLE_vertex_array_object", "glGenVertexArraysAPPLE" } })) {
    }

    const ExtensionFunction<void(GLuint array)> bindVertexArray;

    const ExtensionFunction<void(GLsizei n, const GLuint* arrays)> deleteVertexArrays;

    const ExtensionFunction<void(GLsizei n, GLuint* arrays)> genVertexArrays;
};

} // namespace extension
} // namespace gl
} // namespace mbgl
