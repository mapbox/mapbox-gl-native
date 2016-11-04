#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/platform/log.hpp>

#include <mutex>
#include <string>
#include <vector>
#include <cstring>

namespace mbgl {
namespace gl {
namespace detail {

using Probes = std::vector<ExtensionFunctionBase::Probe>;
using ExtensionFunctions = std::vector<std::pair<glProc*, Probes>>;
ExtensionFunctions& extensionFunctions() {
    static ExtensionFunctions functions;
    return functions;
}

ExtensionFunctionBase::ExtensionFunctionBase(std::initializer_list<Probe> probes) {
    extensionFunctions().emplace_back(&ptr, probes);
}

} // namespace detail

static std::once_flag initializeExtensionsOnce;

const char* getGLString(GLenum name) {
    return reinterpret_cast<const char*>(MBGL_CHECK_ERROR(glGetString(name)));
}

void InitializeExtensions(glProc (*getProcAddress)(const char*)) {
    std::call_once(initializeExtensionsOnce, [getProcAddress] {
#ifndef NDEBUG
        if (const char* vendor = getGLString(GL_VENDOR)) {
            mbgl::Log::Info(mbgl::Event::OpenGL, "Vendor: %s", vendor);
        }
        if (const char* renderer = getGLString(GL_RENDERER)) {
            mbgl::Log::Info(mbgl::Event::OpenGL, "Renderer: %s", renderer);
        }
        if (const char* version = getGLString(GL_VERSION)) {
            mbgl::Log::Info(mbgl::Event::OpenGL, "Version: %s", version);
        }
        if (const char* shaderVersion = getGLString(GL_SHADING_LANGUAGE_VERSION)) {
            mbgl::Log::Info(mbgl::Event::OpenGL, "Shading Language Version: %s", shaderVersion);
        }
#endif

        if (const char* extensions = getGLString(GL_EXTENSIONS)) {
#ifndef NDEBUG
            mbgl::Log::Info(mbgl::Event::OpenGL, "Extensions: %s", extensions);
#endif
            for (auto fn : detail::extensionFunctions()) {
                for (auto probe : fn.second) {
                    if (strstr(extensions, probe.first) != nullptr) {
                        *fn.first = getProcAddress(probe.second);
                        break;
                    }
                }
            }
        }
    });
}

} // namespace gl
} // namespace mbgl
