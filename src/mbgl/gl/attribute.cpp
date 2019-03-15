#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/defines.hpp>

namespace mbgl {
namespace gl {

using namespace platform;

optional<gfx::AttributeBinding> offsetAttributeBinding(const optional<gfx::AttributeBinding>& binding, std::size_t vertexOffset) {
    assert(vertexOffset <= std::numeric_limits<uint32_t>::max());
    if (binding) {
        gfx::AttributeBinding result = *binding;
        result.vertexOffset = static_cast<uint32_t>(vertexOffset);
        return result;
    } else {
        return binding;
    }
}

void bindAttributeLocation(Context& context, ProgramID id, AttributeLocation location, const char* name) {
    // We're using sequentially numberered attribute locations starting with 0. Therefore we can use
    // the location as a proxy for the number of attributes.
    if (location >= context.maximumVertexBindingCount) {
        // Don't bind the location on this hardware since it exceeds the limit (otherwise we'd get
        // an OpenGL error). This means we'll see rendering errors, and possibly slow rendering due
        // to unbound attributes.
    } else {
        MBGL_CHECK_ERROR(glBindAttribLocation(id, location, name));
    }
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
