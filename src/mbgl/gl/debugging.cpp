#include <mbgl/gl/debugging.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/debugging_extension.hpp>

namespace mbgl {
namespace gl {

#ifndef NDEBUG

DebugGroup::DebugGroup(const Context& context_, const std::string& name) : context(context_) {
    if (auto debugging = context.getDebuggingExtension()) {
        if (debugging->pushDebugGroup) {
            MBGL_CHECK_ERROR(debugging->pushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(name.size()), name.c_str()));
        } else if (debugging->pushGroupMarkerEXT) {
            MBGL_CHECK_ERROR(debugging->pushGroupMarkerEXT(GLsizei(name.size() + 1), name.c_str()));
        }
    }
}

DebugGroup::~DebugGroup() {
    if (auto debugging = context.getDebuggingExtension()) {
        if (debugging->popDebugGroup) {
            MBGL_CHECK_ERROR(debugging->popDebugGroup());
        } else if (debugging->popGroupMarkerEXT) {
            MBGL_CHECK_ERROR(debugging->popGroupMarkerEXT());
        }
    }
}

#endif

} // namespace gl
} // namespace mbgl
