#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {
namespace gl {

using namespace platform;

optional<AttributeLocation> queryLocation(ProgramID id, const char* name) {
    GLint attributeLocation = MBGL_CHECK_ERROR(glGetAttribLocation(id, name));
    if (attributeLocation != -1) {
        return attributeLocation;
    } else {
        return {};
    }
}

} // namespace gl
} // namespace mbgl
