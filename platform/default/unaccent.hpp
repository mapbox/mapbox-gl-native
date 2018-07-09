#pragma once

#include <string>

namespace mbgl {
namespace platform {

// Non-locale-aware diacritic folding based on nunicode
// Used as a fallback when locale-aware comparisons aren't available
std::string unaccent(const std::string &string);

} // namespace platform
} // namespace mbgl
