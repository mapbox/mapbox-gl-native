#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {
namespace gl {

void bindAttributeLocation(ProgramID id, AttributeLocation location, const char* name) {
    if (location >= MAX_ATTRIBUTES) {
        throw gl::Error("too many vertex attributes");
    }
    MBGL_CHECK_ERROR(glBindAttribLocation(id, location, name));
}

std::set<std::string> getActiveAttributes(ProgramID id) {
    std::set<std::string> activeAttributes;

    GLint attributeCount;
    MBGL_CHECK_ERROR(glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &attributeCount));

    GLint maxAttributeLength;
    MBGL_CHECK_ERROR(glGetProgramiv(id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeLength));

    std::string attributeName;
    attributeName.resize(maxAttributeLength);

    GLsizei actualLength;
    GLint size;
    GLenum type;

    for (int32_t i = 0; i < attributeCount; i++) {
        MBGL_CHECK_ERROR(glGetActiveAttrib(id, i, maxAttributeLength, &actualLength, &size, &type, &attributeName[0]));
        activeAttributes.emplace(std::string(attributeName, 0, actualLength));
    }

    return activeAttributes;
}

} // namespace gl
} // namespace mbgl
