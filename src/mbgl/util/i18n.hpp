#pragma once

#include <string>

namespace mbgl {
namespace util {
namespace i18n {

/// Returns whether a line break can be inserted after any character in the given string.
/// If false, line breaking should occur on word boundaries instead.
bool allowsIdeographicBreaking(const std::u32string& string);

/// Returns whether a line break can be inserted after the character indicated by the given Unicode codepoint.
bool allowsIdeographicBreaking(uint32_t chr);

} // namespace i18n
} // namespace util
} // namespace mbgl
