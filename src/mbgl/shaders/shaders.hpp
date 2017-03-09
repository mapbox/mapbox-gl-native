#pragma once

#include <string>

namespace mbgl {

class ProgramParameters;

namespace shaders {

std::string fragmentSource(const ProgramParameters&, const char* fragmentSource);
std::string vertexSource(const ProgramParameters&, const char* vertexSource);

} // namespace shaders
} // namespace mbgl
