#include "i18n.hpp"

namespace mbgl {
namespace util {
namespace i18n {

bool allowsIdeographicBreaking(const std::u32string& string) {
    for (uint32_t chr : string) {
        if (!allowsIdeographicBreaking(chr)) {
            return false;
        }
    }
    return true;
}

bool allowsIdeographicBreaking(uint32_t chr) {
    // Return early for characters outside all ideographic ranges.
    if (chr < 0x2E80) return false;

    // CJK Radicals Supplement, Kangxi Radicals, Ideographic Description Characters, CJK Symbols and Punctuation: “⺀” to “〿”
    if (chr >= 0x2E80 && chr <= 0x303F) return true;

    // Hiragana: before “ぁ” to “ゟ”
    if (chr >= 0x3040 && chr <= 0x309F) return true;

    // Katakana: “゠” to “ヿ”
    if (chr >= 0x30A0 && chr <= 0x30FF) return true;

    // CJK Strokes: “㇀” to past “㇣”
    if (chr >= 0x31C0 && chr <= 0x31EF) return true;

    // Katakana Phonetic Extensions: “ㇰ” to “ㇿ”
    if (chr >= 0x31F0 && chr <= 0x31FF) return true;

    // Enclosed CJK Letters and Months, CJK Compatibility: “㈀” to “㏿”
    if (chr >= 0x3200 && chr <= 0x33FF) return true;

    // CJK Unified Ideographs Extension A: “㐀” to past “䶵”
    if (chr >= 0x3400 && chr <= 0x4DBF) return true;

    // CJK Unified Ideographs: “一” to past “鿕”
    if (chr >= 0x4E00 && chr <= 0x9FFF) return true;

    // Yi Syllables, Yi Radicals: “ꀀ” to past “꓆”
    if (chr >= 0xA000 && chr <= 0xA4CF) return true;

    // CJK Compatibility Forms: “︰” to “﹏”
    if (chr >= 0xFE30 && chr <= 0xFE4F) return true;

    // CJK Compatibility Ideographs: “豈” to past “龎”
    if (chr >= 0xF900 && chr <= 0xFAFF) return true;

    // Halfwidth and Fullwidth Forms: before “！” to past “￮”
    if (chr >= 0xFF00 && chr <= 0xFFEF) return true;

    return false;
}

} // namespace i18n
} // namespace util
} // namespace mbgl

