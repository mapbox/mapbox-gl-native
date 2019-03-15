#include <mbgl/programs/gl/shaders.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/util/string.hpp>

#include <cassert>

namespace mbgl {
namespace programs {
namespace gl {

std::string fragmentSource(const ProgramParameters& parameters, const char* fragmentSource) {
    return parameters.getDefines() + fragmentShaderPrelude + fragmentSource;
}

std::string vertexSource(const ProgramParameters& parameters, const char* vertexSource) {
    return parameters.getDefines() + vertexShaderPrelude + vertexSource;
}

std::string programIdentifier(const std::string& vertexSource, const std::string& fragmentSource) {
    std::string result;
    result.reserve((sizeof(size_t) * 2) * 2 + 2); // 2 size_t hex values + "v2"
    result += util::toHex(std::hash<std::string>()(vertexSource));
    result += util::toHex(std::hash<std::string>()(fragmentSource));
    result += "v3";
    return result;
}

} // namespace gl
} // namespace programs
} // namespace mbgl
