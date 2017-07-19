#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {
namespace gl {

AttributeLocation bindAttributeLocation(ProgramID id, AttributeLocation location, const char* name) {
    MBGL_CHECK_ERROR(glBindAttribLocation(id, location, name));
    return location;
}

} // namespace gl
} // namespace mbgl
