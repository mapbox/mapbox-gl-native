#pragma once

#include <string>

namespace mbgl {

class ProgramParameters;

namespace programs {
namespace gl {

std::string programIdentifier(const std::string& defines1,
                              const std::string& defines2,
                              const uint8_t hash1[8],
                              const uint8_t hash2[8]);

} // namespace gl
} // namespace programs
} // namespace mbgl
