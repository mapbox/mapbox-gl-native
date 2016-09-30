#include <mbgl/gl/vertex_array.hpp>

namespace mbgl {
namespace gl {

ExtensionFunction<void(GLuint array)>
    BindVertexArray({ { "GL_ARB_vertex_array_object", "glBindVertexArray" },
                      { "GL_OES_vertex_array_object", "glBindVertexArrayOES" },
                      { "GL_APPLE_vertex_array_object", "glBindVertexArrayAPPLE" } });

ExtensionFunction<void(GLsizei n, const GLuint* arrays)>
    DeleteVertexArrays({ { "GL_ARB_vertex_array_object", "glDeleteVertexArrays" },
                         { "GL_OES_vertex_array_object", "glDeleteVertexArraysOES" },
                         { "GL_APPLE_vertex_array_object", "glDeleteVertexArraysAPPLE" } });

ExtensionFunction<void(GLsizei n, GLuint* arrays)>
    GenVertexArrays({ { "GL_ARB_vertex_array_object", "glGenVertexArrays" },
                      { "GL_OES_vertex_array_object", "glGenVertexArraysOES" },
                      { "GL_APPLE_vertex_array_object", "glGenVertexArraysAPPLE" } });

} // namespace gl
} // namespace mbgl
