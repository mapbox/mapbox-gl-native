#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {
namespace gl {

AttributeLocation bindAttributeLocation(ProgramID id, AttributeLocation location, const char* name) {
    MBGL_CHECK_ERROR(glBindAttribLocation(id, location, name));
    return location;
}

void bindAttribute(AttributeLocation location,
                   std::size_t count,
                   DataType type,
                   std::size_t vertexSize,
                   std::size_t vertexOffset,
                   std::size_t attributeOffset) {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttribPointer(
        location,
        static_cast<GLint>(count),
        static_cast<GLenum>(type),
        GL_FALSE,
        static_cast<GLsizei>(vertexSize),
        reinterpret_cast<GLvoid*>(attributeOffset + (vertexSize * vertexOffset))));
}

} // namespace gl
} // namespace mbgl
