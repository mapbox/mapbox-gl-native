#include <mbgl/programs/gl/shaders.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/util/string.hpp>

#include <cassert>

namespace mbgl {
namespace programs {
namespace gl {

std::string programIdentifier(const std::string& defines1,
                              const std::string& defines2,
                              const uint8_t hash1[8],
                              const uint8_t hash2[8]) {
    std::string result;
    result.reserve(8 + 8 + (sizeof(size_t) * 2) * 2 + 2);
    result.append(util::toHex(static_cast<uint64_t>(std::hash<std::string>()(defines1))));
    result.append(util::toHex(static_cast<uint64_t>(std::hash<std::string>()(defines2))));
    result.append(hash1, hash2 + 8);
    result.append(hash2, hash2 + 8);
    result.append("v3");
    return result;
}

} // namespace gl
} // namespace programs
} // namespace mbgl
