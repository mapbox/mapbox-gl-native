#pragma once

#include <string>

namespace mbgl {
namespace util {
namespace i18n {

/** Returns whether a line break can be inserted after the character indicated
    by the given Unicode codepoint due to word breaking. */
bool allowsWordBreaking(char16_t chr);

/** Returns whether the given string can be displayed with letter-spacing.
    False for Arabic scripts, where letter-spacing will break ligatures. */
bool allowsLetterSpacing(const std::u16string& string);

/** Returns whether a line break can be inserted after any character in the
    given string. If false, line breaking should occur on word boundaries
    instead. */
bool allowsIdeographicBreaking(const std::u16string& string);

/** Returns whether a line break can be inserted after the character indicated
    by the given Unicode codepoint due to ideographic breaking. */
bool allowsIdeographicBreaking(char16_t chr);

/** Conservative set of characters expected to have relatively fixed sizes and
    advances */
bool allowsFixedWidthGlyphGeneration(char16_t chr);

/** Returns whether any substring of the given string can be drawn as vertical
    text with upright glyphs. */
bool allowsVerticalWritingMode(const std::u16string& string);

/** Returns true if the given Unicode codepoint identifies a character with
    upright orientation.

    A character has upright orientation if it is drawn upright (unrotated)
    whether the line is oriented horizontally or vertically, even if both
    adjacent characters can be rotated. For example, a Chinese character is
    always drawn upright. An uprightly oriented character causes an adjacent
    “neutral” character to be drawn upright as well. */
bool hasUprightVerticalOrientation(char16_t chr);

/** Returns true if the given Unicode codepoint identifies a character with
    neutral orientation.

    A character has neutral orientation if it may be drawn rotated or unrotated
    when the line is oriented vertically, depending on the orientation of the
    adjacent characters. For example, along a verticlly oriented line, the
    vulgar fraction ½ is drawn upright among Chinese characters but rotated
    among Latin letters. A neutrally oriented character does not influence
    whether an adjacent character is drawn upright or rotated.
 */
bool hasNeutralVerticalOrientation(char16_t chr);

/** Returns true if the given Unicode codepoint identifies a character with
    rotated orientation.

    A character has rotated orientation if it is drawn rotated when the line is
    oriented vertically, even if both adjacent characters are upright. For
    example, a Latin letter is drawn rotated along a vertical line. A rotated
    character causes an adjacent “neutral” character to be drawn rotated as
    well.
 */
bool hasRotatedVerticalOrientation(char16_t chr);

/** Returns a copy of the given string with punctuation characters replaced with
    their vertical forms wherever applicable. */
std::u16string verticalizePunctuation(const std::u16string& input);

/** Returns the form of the given character appropriate for vertical text.

    @return The character’s specialized vertical form; 0 if not applicable. */
char16_t verticalizePunctuation(char16_t chr);
    
bool isStringInSupportedScript(const std::string& input);

} // namespace i18n
} // namespace util
} // namespace mbgl
