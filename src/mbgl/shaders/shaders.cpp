#include <mbgl/shaders/shaders.hpp>
#include <mbgl/shaders/preludes.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/util/string.hpp>

#include <cassert>

namespace mbgl {
namespace shaders {

std::string fragmentSource(const ProgramParameters& parameters, const char* fragmentSource) {
    return parameters.getDefines() + fragmentPrelude + fragmentSource;
}

std::string vertexSource(const ProgramParameters& parameters, const char* vertexSource) {
    return parameters.getDefines() + vertexPrelude + vertexSource;
}

std::string programIdentifier(const std::string& vertexSource, const std::string& fragmentSource) {
    std::string result;
    result.reserve((sizeof(size_t) * 2) * 2 + 2); // 2 size_t hex values + "v2"
    result += util::toHex(std::hash<std::string>()(vertexSource));
    result += util::toHex(std::hash<std::string>()(fragmentSource));
    result += "v2";
    return result;
}

} // namespace shaders
} // namespace mbgl
