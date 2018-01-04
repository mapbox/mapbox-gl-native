#include <mbgl/shaders/shaders.hpp>
#include <mbgl/shaders/preludes.hpp>
#include <mbgl/programs/program_parameters.hpp>

#include <cassert>
#include <sstream>
#include <iomanip>

namespace mbgl {
namespace shaders {

std::string fragmentSource(const ProgramParameters& parameters, const char* fragmentSource) {
    return parameters.getDefines() + fragmentPrelude + fragmentSource;
}

std::string vertexSource(const ProgramParameters& parameters, const char* vertexSource) {
    return parameters.getDefines() + vertexPrelude + vertexSource;
}

std::string programIdentifier(const std::string& vertexSource, const std::string& fragmentSource) {
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(sizeof(size_t) * 2) << std::hex;
    ss << std::hash<std::string>()(vertexSource);
    ss << std::hash<std::string>()(fragmentSource);
    ss << "v2";
    return ss.str();
}

} // namespace shaders
} // namespace mbgl
