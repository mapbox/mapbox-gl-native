#pragma once

#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {
namespace gl {

extern ExtensionFunction<void(GLuint array)> BindVertexArray;
extern ExtensionFunction<void(GLsizei n, const GLuint* arrays)> DeleteVertexArrays;
extern ExtensionFunction<void(GLsizei n, GLuint* arrays)> GenVertexArrays;

} // namespace gl
} // namespace mbgl