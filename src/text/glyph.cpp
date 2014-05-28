#include <llmr/text/glyph.hpp>

namespace llmr {

// Note: this only works for the BMP
// Note: we could use a binary lookup table to get averaged constant time lookups, however,
// most of our lookups are going to be within the first 3 ranges listed here, so this is
// likely faster.
GlyphRange getGlyphRange(uint32_t glyph) {
    if (glyph <= 127) return { 0, 127 }; // Basic Latin
    if (glyph <= 255) return { 128, 255 }; // Latin-1 Supplement
    if (glyph >= 8192 && glyph <= 8303) return { 8192, 8303 }; // General Punctuation
    if (glyph <= 383) return { 256, 383 }; // Latin Extended-A
    if (glyph <= 591) return { 384, 591 }; // Latin Extended-B
    if (glyph <= 687) return { 592, 687 }; // IPA Extensions
    if (glyph <= 767) return { 688, 767 }; // Spacing Modifier Letters
    if (glyph <= 879) return { 768, 879 }; // Combining Diacritical Marks
    if (glyph <= 1023) return { 880, 1023 }; // Greek
    if (glyph <= 1279) return { 1024, 1279 }; // Cyrillic
    if (glyph <= 1423) return { 1280, 1423 }; // Armenian
    if (glyph <= 1535) return { 1424, 1535 }; // Hebrew
    if (glyph <= 1791) return { 1536, 1791 }; // Arabic
    if (glyph <= 1871) return { 1792, 1871 }; // Syriac
    if (glyph <= 1983) return { 1872, 1983 }; // Thaana
    if (glyph <= 2431) return { 1984, 2431 }; // Devanagari
    if (glyph <= 2559) return { 2432, 2559 }; // Bengali
    if (glyph <= 2687) return { 2560, 2687 }; // Gurmukhi
    if (glyph <= 2815) return { 2688, 2815 }; // Gujarati
    if (glyph <= 2943) return { 2816, 2943 }; // Oriya
    if (glyph <= 3071) return { 2944, 3071 }; // Tamil
    if (glyph <= 3199) return { 3072, 3199 }; // Telugu
    if (glyph <= 3327) return { 3200, 3327 }; // Kannada
    if (glyph <= 3455) return { 3328, 3455 }; // Malayalam
    if (glyph <= 3583) return { 3456, 3583 }; // Sinhala
    if (glyph <= 3711) return { 3584, 3711 }; // Thai
    if (glyph <= 3839) return { 3712, 3839 }; // Lao
    if (glyph <= 4095) return { 3840, 4095 }; // Tibetan
    if (glyph <= 4255) return { 4096, 4255 }; // Myanmar
    if (glyph <= 4351) return { 4256, 4351 }; // Georgian
    if (glyph <= 4607) return { 4352, 4607 }; // Hangul Jamo
    if (glyph <= 4991) return { 4608, 4991 }; // Ethiopic
    if (glyph <= 5119) return { 4992, 5119 }; // Cherokee
    if (glyph <= 5759) return { 5120, 5759 }; // Unified Canadian Aboriginal Syllabics
    if (glyph <= 5791) return { 5760, 5791 }; // Ogham
    if (glyph <= 5887) return { 5792, 5887 }; // Runic
    if (glyph <= 6143) return { 5888, 6143 }; // Khmer
    if (glyph <= 6319) return { 6144, 6319 }; // Mongolian
    if (glyph <= 7935) return { 6320, 7935 }; // Latin Extended Additional
    if (glyph <= 8191) return { 7936, 8191 }; // Greek Extended
    if (glyph <= 8351) return { 8304, 8351 }; // Superscripts and Subscripts
    if (glyph <= 8399) return { 8352, 8399 }; // Currency Symbols
    if (glyph <= 8447) return { 8400, 8447 }; // Combining Marks for Symbols
    if (glyph <= 8527) return { 8448, 8527 }; // Letterlike Symbols
    if (glyph <= 8591) return { 8528, 8591 }; // Number Forms
    if (glyph <= 8703) return { 8592, 8703 }; // Arrows
    if (glyph <= 8959) return { 8704, 8959 }; // Mathematical Operators
    if (glyph <= 9215) return { 8960, 9215 }; // Miscellaneous Technical
    if (glyph <= 9279) return { 9216, 9279 }; // Control Pictures
    if (glyph <= 9311) return { 9280, 9311 }; // Optical Character Recognition
    if (glyph <= 9471) return { 9312, 9471 }; // Enclosed Alphanumerics
    if (glyph <= 9599) return { 9472, 9599 }; // Box Drawing
    if (glyph <= 9631) return { 9600, 9631 }; // Block Elements
    if (glyph <= 9727) return { 9632, 9727 }; // Geometric Shapes
    if (glyph <= 9983) return { 9728, 9983 }; // Miscellaneous Symbols
    if (glyph <= 10175) return { 9984, 10175 }; // Dingbats
    if (glyph <= 10495) return { 10176, 10495 }; // Braille Patterns
    if (glyph <= 12031) return { 10496, 12031 }; // CJK Radicals Supplement
    if (glyph <= 12255) return { 12032, 12255 }; // Kangxi Radicals
    if (glyph <= 12287) return { 12256, 12287 }; // Ideographic Description Characters
    if (glyph <= 12351) return { 12288, 12351 }; // CJK Symbols and Punctuation
    if (glyph <= 12447) return { 12352, 12447 }; // Hiragana
    if (glyph <= 12543) return { 12448, 12543 }; // Katakana
    if (glyph <= 12591) return { 12544, 12591 }; // Bopomofo
    if (glyph <= 12687) return { 12592, 12687 }; // Hangul Compatibility Jamo
    if (glyph <= 12703) return { 12688, 12703 }; // Kanbun
    if (glyph <= 12735) return { 12704, 12735 }; // Bopomofo Extended
    if (glyph <= 13055) return { 12736, 13055 }; // Enclosed CJK Letters and Months
    if (glyph <= 13311) return { 13056, 13311 }; // CJK Compatibility
    if (glyph <= 19893) return { 13312, 19893 }; // CJK Unified Ideographs Extension A
    if (glyph <= 40959) return { 19894, 40959 }; // CJK Unified Ideographs
    if (glyph <= 42127) return { 40960, 42127 }; // Yi Syllables
    if (glyph <= 42191) return { 42128, 42191 }; // Yi Radicals
    if (glyph <= 55203) return { 42192, 55203 }; // Hangul Syllables
    if (glyph <= 56191) return { 55204, 56191 }; // High Surrogates
    if (glyph <= 56319) return { 56192, 56319 }; // High Private Use Surrogates
    if (glyph <= 57343) return { 56320, 57343 }; // Low Surrogates
    if (glyph <= 63743) return { 57344, 63743 }; // Private Use
    if (glyph <= 64255) return { 63744, 64255 }; // CJK Compatibility Ideographs
    if (glyph <= 64335) return { 64256, 64335 }; // Alphabetic Presentation Forms
    if (glyph <= 65023) return { 64336, 65023 }; // Arabic Presentation Forms-A
    if (glyph <= 65071) return { 65024, 65071 }; // Combining Half Marks
    if (glyph <= 65103) return { 65072, 65103 }; // CJK Compatibility Forms
    if (glyph <= 65135) return { 65104, 65135 }; // Small Form Variants
    if (glyph <= 65278) return { 65136, 65278 }; // Arabic Presentation Forms-B
    if (glyph <= 65279) return { 65279, 65279 }; // Specials
    if (glyph <= 65519) return { 65280, 65519 }; // Halfwidth and Fullwidth Forms
    if (glyph <= 65533) return { 65520, 65533 }; // Specials
    return { 0, 0 };
}

}
