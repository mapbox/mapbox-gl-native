#include <mbgl/gl/command_encoder.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/debugging_extension.hpp>
#include <mbgl/platform/gl_functions.hpp>

#include <cstring>

namespace mbgl {
namespace gl {

CommandEncoder::~CommandEncoder() {
    const auto debugGroup(createDebugGroup("cleanup"));
    context.performCleanup();
}

void CommandEncoder::pushDebugGroup(const char* name) {
    (void)name;
#ifndef NDEBUG
    if (auto debugging = context.getDebuggingExtension()) {
        if (debugging->pushDebugGroup) {
            MBGL_CHECK_ERROR(debugging->pushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0,
                                                       platform::GLsizei(strlen(name)), name));
        } else if (debugging->pushGroupMarkerEXT) {
            MBGL_CHECK_ERROR(debugging->pushGroupMarkerEXT(platform::GLsizei(strlen(name) + 1), name));
        }
    }
#endif
}

void CommandEncoder::popDebugGroup() {
#ifndef NDEBUG
    if (auto debugging = context.getDebuggingExtension()) {
        if (debugging->popDebugGroup) {
            MBGL_CHECK_ERROR(debugging->popDebugGroup());
        } else if (debugging->popGroupMarkerEXT) {
            MBGL_CHECK_ERROR(debugging->popGroupMarkerEXT());
        }
    }
#endif
}

} // namespace gl
} // namespace mbgl
