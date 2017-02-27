#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/gl.hpp>

#include <string>
#include <vector>
#include <cstring>

namespace mbgl {
namespace gl {
namespace detail {

using Probes = std::vector<ExtensionFunctionBase::Probe>;
using ExtensionFunctions = std::vector<std::pair<ProcAddress*, Probes>>;
ExtensionFunctions& extensionFunctions() {
    static ExtensionFunctions functions;
    return functions;
}

ExtensionFunctionBase::ExtensionFunctionBase(std::initializer_list<Probe> probes) {
    extensionFunctions().emplace_back(&ptr, probes);
}

} // namespace detail

void initializeExtensions(const std::function<ProcAddress(const char*)>& getProcAddress) {
    if (const char* extensions =
            reinterpret_cast<const char*>(MBGL_CHECK_ERROR(glGetString(GL_EXTENSIONS)))) {
        for (auto fn : detail::extensionFunctions()) {
            for (auto probe : fn.second) {
                if (strstr(extensions, probe.first) != nullptr) {
                    *fn.first = getProcAddress(probe.second);
                    break;
                }
            }
        }
    }
}

} // namespace gl
} // namespace mbgl
