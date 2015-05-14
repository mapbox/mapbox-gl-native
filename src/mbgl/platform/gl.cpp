#include <mbgl/platform/gl.hpp>

#include <mutex>

namespace mbgl {
namespace gl {

std::vector<ExtensionFunctionBase*>& ExtensionFunctionBase::functions() {
    static std::vector<ExtensionFunctionBase*> functions;
    return functions;
}

static std::once_flag initializeExtensionsOnce;

void InitializeExtensions(glProc (*getProcAddress)(const char *)) {
    std::call_once(initializeExtensionsOnce, [getProcAddress] {
        const char * extensionsPtr = reinterpret_cast<const char *>(
            MBGL_CHECK_ERROR(glGetString(GL_EXTENSIONS)));

        if (!extensionsPtr)
            return;

        const std::string extensions = extensionsPtr;
        for (auto fn : ExtensionFunctionBase::functions()) {
            for (auto probe : fn->probes) {
                if (extensions.find(probe.first) != std::string::npos) {
                    fn->ptr = getProcAddress(probe.second);
                    break;
                }
            }
        }
    });
}

void checkError(const char *cmd, const char *file, int line) {
    const GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        const char *error = nullptr;
        switch (err) {
            case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
            case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
#ifdef GL_STACK_UNDERFLOW
            case GL_STACK_UNDERFLOW:  error = "STACK_UNDERFLOW";  break;
#endif
#ifdef GL_STACK_OVERFLOW
            case GL_STACK_OVERFLOW:  error = "STACK_OVERFLOW";  break;
#endif
            default: error = "(unknown)"; break;
        }

        throw ::mbgl::gl::Error(err, std::string(cmd) + ": Error GL_" + error + " - " + file + ":" + std::to_string(line));
    }
}

}
}
