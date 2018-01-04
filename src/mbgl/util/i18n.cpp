#include "i18n.hpp"

#include <algorithm>
#include <map>

namespace {

/** Defines a function that returns true if a codepoint is in a named block.
    @param name The name of the block in CamelCase.
    @param first The first codepoint in the block, inclusive.
    @param last The last codepoint in the block, inclusive.
 */
#define DEFINE_IS_IN_UNICODE_BLOCK(name, first, last)                                              \
    inline bool isIn##name(char16_t codepoint) {                                                   \
        return codepoint >= first && codepoint <= last;                                            \
    }

// The following table comes from <http://www.unicode.org/Public/10.0.0/ucd/Blocks.txt>.
// Keep it synchronized with <http://www.unicode.org/Public/UCD/latest/ucd/Blocks.txt>.

// DEFINE_IS_IN_UNICODE_BLOCK(BasicLatin, 0x0000, 0x007F)
DEFINE_IS_IN_UNICODE_BLOCK(Latin1Supplement, 0x0080, 0x00FF)
// DEFINE_IS_IN_UNICODE_BLOCK(LatinExtendedA, 0x0100, 0x017F)
// DEFINE_IS_IN_UNICODE_BLOCK(LatinExtendedB, 0x0180, 0x024F)
// DEFINE_IS_IN_UNICODE_BLOCK(IPAExtensions, 0x0250, 0x02AF)
// DEFINE_IS_IN_UNICODE_BLOCK(SpacingModifierLetters, 0x02B0, 0x02FF)
// DEFINE_IS_IN_UNICODE_BLOCK(CombiningDiacriticalMarks, 0x0300, 0x036F)
// DEFINE_IS_IN_UNICODE_BLOCK(GreekandCoptic, 0x0370, 0x03FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Cyrillic, 0x0400, 0x04FF)
// DEFINE_IS_IN_UNICODE_BLOCK(CyrillicSupplement, 0x0500, 0x052F)
// DEFINE_IS_IN_UNICODE_BLOCK(Armenian, 0x0530, 0x058F)
// DEFINE_IS_IN_UNICODE_BLOCK(Hebrew, 0x0590, 0x05FF)
DEFINE_IS_IN_UNICODE_BLOCK(Arabic, 0x0600, 0x06FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Syriac, 0x0700, 0x074F)
DEFINE_IS_IN_UNICODE_BLOCK(ArabicSupplement, 0x0750, 0x077F)
// DEFINE_IS_IN_UNICODE_BLOCK(Thaana, 0x0780, 0x07BF)
// DEFINE_IS_IN_UNICODE_BLOCK(NKo, 0x07C0, 0x07FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Samaritan, 0x0800, 0x083F)
// DEFINE_IS_IN_UNICODE_BLOCK(Mandaic, 0x0840, 0x085F)
// DEFINE_IS_IN_UNICODE_BLOCK(Syriac Supplement, 0x0860, 0x086F)
DEFINE_IS_IN_UNICODE_BLOCK(ArabicExtendedA, 0x08A0, 0x08FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Devanagari, 0x0900, 0x097F)
// DEFINE_IS_IN_UNICODE_BLOCK(Bengali, 0x0980, 0x09FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Gurmukhi, 0x0A00, 0x0A7F)
// DEFINE_IS_IN_UNICODE_BLOCK(Gujarati, 0x0A80, 0x0AFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Oriya, 0x0B00, 0x0B7F)
// DEFINE_IS_IN_UNICODE_BLOCK(Tamil, 0x0B80, 0x0BFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Telugu, 0x0C00, 0x0C7F)
// DEFINE_IS_IN_UNICODE_BLOCK(Kannada, 0x0C80, 0x0CFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Malayalam, 0x0D00, 0x0D7F)
// DEFINE_IS_IN_UNICODE_BLOCK(Sinhala, 0x0D80, 0x0DFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Thai, 0x0E00, 0x0E7F)
// DEFINE_IS_IN_UNICODE_BLOCK(Lao, 0x0E80, 0x0EFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Tibetan, 0x0F00, 0x0FFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Myanmar, 0x1000, 0x109F)
// DEFINE_IS_IN_UNICODE_BLOCK(Georgian, 0x10A0, 0x10FF)
DEFINE_IS_IN_UNICODE_BLOCK(HangulJamo, 0x1100, 0x11FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Ethiopic, 0x1200, 0x137F)
// DEFINE_IS_IN_UNICODE_BLOCK(EthiopicSupplement, 0x1380, 0x139F)
// DEFINE_IS_IN_UNICODE_BLOCK(Cherokee, 0x13A0, 0x13FF)
DEFINE_IS_IN_UNICODE_BLOCK(UnifiedCanadianAboriginalSyllabics, 0x1400, 0x167F)
// DEFINE_IS_IN_UNICODE_BLOCK(Ogham, 0x1680, 0x169F)
// DEFINE_IS_IN_UNICODE_BLOCK(Runic, 0x16A0, 0x16FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Tagalog, 0x1700, 0x171F)
// DEFINE_IS_IN_UNICODE_BLOCK(Hanunoo, 0x1720, 0x173F)
// DEFINE_IS_IN_UNICODE_BLOCK(Buhid, 0x1740, 0x175F)
// DEFINE_IS_IN_UNICODE_BLOCK(Tagbanwa, 0x1760, 0x177F)
// DEFINE_IS_IN_UNICODE_BLOCK(Khmer, 0x1780, 0x17FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Mongolian, 0x1800, 0x18AF)
DEFINE_IS_IN_UNICODE_BLOCK(UnifiedCanadianAboriginalSyllabicsExtended, 0x18B0, 0x18FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Limbu, 0x1900, 0x194F)
// DEFINE_IS_IN_UNICODE_BLOCK(TaiLe, 0x1950, 0x197F)
// DEFINE_IS_IN_UNICODE_BLOCK(NewTaiLue, 0x1980, 0x19DF)
// DEFINE_IS_IN_UNICODE_BLOCK(KhmerSymbols, 0x19E0, 0x19FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Buginese, 0x1A00, 0x1A1F)
// DEFINE_IS_IN_UNICODE_BLOCK(TaiTham, 0x1A20, 0x1AAF)
// DEFINE_IS_IN_UNICODE_BLOCK(CombiningDiacriticalMarksExtended, 0x1AB0, 0x1AFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Balinese, 0x1B00, 0x1B7F)
// DEFINE_IS_IN_UNICODE_BLOCK(Sundanese, 0x1B80, 0x1BBF)
// DEFINE_IS_IN_UNICODE_BLOCK(Batak, 0x1BC0, 0x1BFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Lepcha, 0x1C00, 0x1C4F)
// DEFINE_IS_IN_UNICODE_BLOCK(OlChiki, 0x1C50, 0x1C7F)
// DEFINE_IS_IN_UNICODE_BLOCK(CyrillicExtendedC, 0x1C80, 0x1C8F)
// DEFINE_IS_IN_UNICODE_BLOCK(SundaneseSupplement, 0x1CC0, 0x1CCF)
// DEFINE_IS_IN_UNICODE_BLOCK(VedicExtensions, 0x1CD0, 0x1CFF)
// DEFINE_IS_IN_UNICODE_BLOCK(PhoneticExtensions, 0x1D00, 0x1D7F)
// DEFINE_IS_IN_UNICODE_BLOCK(PhoneticExtensionsSupplement, 0x1D80, 0x1DBF)
// DEFINE_IS_IN_UNICODE_BLOCK(CombiningDiacriticalMarksSupplement, 0x1DC0, 0x1DFF)
// DEFINE_IS_IN_UNICODE_BLOCK(LatinExtendedAdditional, 0x1E00, 0x1EFF)
// DEFINE_IS_IN_UNICODE_BLOCK(GreekExtended, 0x1F00, 0x1FFF)
DEFINE_IS_IN_UNICODE_BLOCK(GeneralPunctuation, 0x2000, 0x206F)
// DEFINE_IS_IN_UNICODE_BLOCK(SuperscriptsandSubscripts, 0x2070, 0x209F)
// DEFINE_IS_IN_UNICODE_BLOCK(CurrencySymbols, 0x20A0, 0x20CF)
// DEFINE_IS_IN_UNICODE_BLOCK(CombiningDiacriticalMarksforSymbols, 0x20D0, 0x20FF)
DEFINE_IS_IN_UNICODE_BLOCK(LetterlikeSymbols, 0x2100, 0x214F)
DEFINE_IS_IN_UNICODE_BLOCK(NumberForms, 0x2150, 0x218F)
// DEFINE_IS_IN_UNICODE_BLOCK(Arrows, 0x2190, 0x21FF)
// DEFINE_IS_IN_UNICODE_BLOCK(MathematicalOperators, 0x2200, 0x22FF)
DEFINE_IS_IN_UNICODE_BLOCK(MiscellaneousTechnical, 0x2300, 0x23FF)
DEFINE_IS_IN_UNICODE_BLOCK(ControlPictures, 0x2400, 0x243F)
DEFINE_IS_IN_UNICODE_BLOCK(OpticalCharacterRecognition, 0x2440, 0x245F)
DEFINE_IS_IN_UNICODE_BLOCK(EnclosedAlphanumerics, 0x2460, 0x24FF)
// DEFINE_IS_IN_UNICODE_BLOCK(BoxDrawing, 0x2500, 0x257F)
// DEFINE_IS_IN_UNICODE_BLOCK(BlockElements, 0x2580, 0x259F)
DEFINE_IS_IN_UNICODE_BLOCK(GeometricShapes, 0x25A0, 0x25FF)
DEFINE_IS_IN_UNICODE_BLOCK(MiscellaneousSymbols, 0x2600, 0x26FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Dingbats, 0x2700, 0x27BF)
// DEFINE_IS_IN_UNICODE_BLOCK(MiscellaneousMathematicalSymbolsA, 0x27C0, 0x27EF)
// DEFINE_IS_IN_UNICODE_BLOCK(SupplementalArrowsA, 0x27F0, 0x27FF)
// DEFINE_IS_IN_UNICODE_BLOCK(BraillePatterns, 0x2800, 0x28FF)
// DEFINE_IS_IN_UNICODE_BLOCK(SupplementalArrowsB, 0x2900, 0x297F)
// DEFINE_IS_IN_UNICODE_BLOCK(MiscellaneousMathematicalSymbolsB, 0x2980, 0x29FF)
// DEFINE_IS_IN_UNICODE_BLOCK(SupplementalMathematicalOperators, 0x2A00, 0x2AFF)
// DEFINE_IS_IN_UNICODE_BLOCK(MiscellaneousSymbolsandArrows, 0x2B00, 0x2BFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Glagolitic, 0x2C00, 0x2C5F)
// DEFINE_IS_IN_UNICODE_BLOCK(LatinExtendedC, 0x2C60, 0x2C7F)
// DEFINE_IS_IN_UNICODE_BLOCK(Coptic, 0x2C80, 0x2CFF)
// DEFINE_IS_IN_UNICODE_BLOCK(GeorgianSupplement, 0x2D00, 0x2D2F)
// DEFINE_IS_IN_UNICODE_BLOCK(Tifinagh, 0x2D30, 0x2D7F)
// DEFINE_IS_IN_UNICODE_BLOCK(EthiopicExtended, 0x2D80, 0x2DDF)
// DEFINE_IS_IN_UNICODE_BLOCK(CyrillicExtendedA, 0x2DE0, 0x2DFF)
// DEFINE_IS_IN_UNICODE_BLOCK(SupplementalPunctuation, 0x2E00, 0x2E7F)
DEFINE_IS_IN_UNICODE_BLOCK(CJKRadicalsSupplement, 0x2E80, 0x2EFF)
DEFINE_IS_IN_UNICODE_BLOCK(KangxiRadicals, 0x2F00, 0x2FDF)
DEFINE_IS_IN_UNICODE_BLOCK(IdeographicDescriptionCharacters, 0x2FF0, 0x2FFF)
DEFINE_IS_IN_UNICODE_BLOCK(CJKSymbolsandPunctuation, 0x3000, 0x303F)
DEFINE_IS_IN_UNICODE_BLOCK(Hiragana, 0x3040, 0x309F)
DEFINE_IS_IN_UNICODE_BLOCK(Katakana, 0x30A0, 0x30FF)
DEFINE_IS_IN_UNICODE_BLOCK(Bopomofo, 0x3100, 0x312F)
DEFINE_IS_IN_UNICODE_BLOCK(HangulCompatibilityJamo, 0x3130, 0x318F)
DEFINE_IS_IN_UNICODE_BLOCK(Kanbun, 0x3190, 0x319F)
DEFINE_IS_IN_UNICODE_BLOCK(BopomofoExtended, 0x31A0, 0x31BF)
DEFINE_IS_IN_UNICODE_BLOCK(CJKStrokes, 0x31C0, 0x31EF)
DEFINE_IS_IN_UNICODE_BLOCK(KatakanaPhoneticExtensions, 0x31F0, 0x31FF)
DEFINE_IS_IN_UNICODE_BLOCK(EnclosedCJKLettersandMonths, 0x3200, 0x32FF)
DEFINE_IS_IN_UNICODE_BLOCK(CJKCompatibility, 0x3300, 0x33FF)
DEFINE_IS_IN_UNICODE_BLOCK(CJKUnifiedIdeographsExtensionA, 0x3400, 0x4DBF)
DEFINE_IS_IN_UNICODE_BLOCK(YijingHexagramSymbols, 0x4DC0, 0x4DFF)
DEFINE_IS_IN_UNICODE_BLOCK(CJKUnifiedIdeographs, 0x4E00, 0x9FFF)
DEFINE_IS_IN_UNICODE_BLOCK(YiSyllables, 0xA000, 0xA48F)
DEFINE_IS_IN_UNICODE_BLOCK(YiRadicals, 0xA490, 0xA4CF)
// DEFINE_IS_IN_UNICODE_BLOCK(Lisu, 0xA4D0, 0xA4FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Vai, 0xA500, 0xA63F)
// DEFINE_IS_IN_UNICODE_BLOCK(CyrillicExtendedB, 0xA640, 0xA69F)
// DEFINE_IS_IN_UNICODE_BLOCK(Bamum, 0xA6A0, 0xA6FF)
// DEFINE_IS_IN_UNICODE_BLOCK(ModifierToneLetters, 0xA700, 0xA71F)
// DEFINE_IS_IN_UNICODE_BLOCK(LatinExtendedD, 0xA720, 0xA7FF)
// DEFINE_IS_IN_UNICODE_BLOCK(SylotiNagri, 0xA800, 0xA82F)
// DEFINE_IS_IN_UNICODE_BLOCK(CommonIndicNumberForms, 0xA830, 0xA83F)
// DEFINE_IS_IN_UNICODE_BLOCK(Phagspa, 0xA840, 0xA87F)
// DEFINE_IS_IN_UNICODE_BLOCK(Saurashtra, 0xA880, 0xA8DF)
// DEFINE_IS_IN_UNICODE_BLOCK(DevanagariExtended, 0xA8E0, 0xA8FF)
// DEFINE_IS_IN_UNICODE_BLOCK(KayahLi, 0xA900, 0xA92F)
// DEFINE_IS_IN_UNICODE_BLOCK(Rejang, 0xA930, 0xA95F)
DEFINE_IS_IN_UNICODE_BLOCK(HangulJamoExtendedA, 0xA960, 0xA97F)
// DEFINE_IS_IN_UNICODE_BLOCK(Javanese, 0xA980, 0xA9DF)
// DEFINE_IS_IN_UNICODE_BLOCK(MyanmarExtendedB, 0xA9E0, 0xA9FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Cham, 0xAA00, 0xAA5F)
// DEFINE_IS_IN_UNICODE_BLOCK(MyanmarExtendedA, 0xAA60, 0xAA7F)
// DEFINE_IS_IN_UNICODE_BLOCK(TaiViet, 0xAA80, 0xAADF)
// DEFINE_IS_IN_UNICODE_BLOCK(MeeteiMayekExtensions, 0xAAE0, 0xAAFF)
// DEFINE_IS_IN_UNICODE_BLOCK(EthiopicExtendedA, 0xAB00, 0xAB2F)
// DEFINE_IS_IN_UNICODE_BLOCK(LatinExtendedE, 0xAB30, 0xAB6F)
// DEFINE_IS_IN_UNICODE_BLOCK(CherokeeSupplement, 0xAB70, 0xABBF)
// DEFINE_IS_IN_UNICODE_BLOCK(MeeteiMayek, 0xABC0, 0xABFF)
DEFINE_IS_IN_UNICODE_BLOCK(HangulSyllables, 0xAC00, 0xD7AF)
DEFINE_IS_IN_UNICODE_BLOCK(HangulJamoExtendedB, 0xD7B0, 0xD7FF)
// DEFINE_IS_IN_UNICODE_BLOCK(HighSurrogates, 0xD800, 0xDB7F)
// DEFINE_IS_IN_UNICODE_BLOCK(HighPrivateUseSurrogates, 0xDB80, 0xDBFF)
// DEFINE_IS_IN_UNICODE_BLOCK(LowSurrogates, 0xDC00, 0xDFFF)
DEFINE_IS_IN_UNICODE_BLOCK(PrivateUseArea, 0xE000, 0xF8FF)
DEFINE_IS_IN_UNICODE_BLOCK(CJKCompatibilityIdeographs, 0xF900, 0xFAFF)
// DEFINE_IS_IN_UNICODE_BLOCK(AlphabeticPresentationForms, 0xFB00, 0xFB4F)
DEFINE_IS_IN_UNICODE_BLOCK(ArabicPresentationFormsA, 0xFB50, 0xFDFF)
// DEFINE_IS_IN_UNICODE_BLOCK(VariationSelectors, 0xFE00, 0xFE0F)
DEFINE_IS_IN_UNICODE_BLOCK(VerticalForms, 0xFE10, 0xFE1F)
// DEFINE_IS_IN_UNICODE_BLOCK(CombiningHalfMarks, 0xFE20, 0xFE2F)
DEFINE_IS_IN_UNICODE_BLOCK(CJKCompatibilityForms, 0xFE30, 0xFE4F)
DEFINE_IS_IN_UNICODE_BLOCK(SmallFormVariants, 0xFE50, 0xFE6F)
DEFINE_IS_IN_UNICODE_BLOCK(ArabicPresentationFormsB, 0xFE70, 0xFEFF)
DEFINE_IS_IN_UNICODE_BLOCK(HalfwidthandFullwidthForms, 0xFF00, 0xFFEF)
// DEFINE_IS_IN_UNICODE_BLOCK(Specials, 0xFFF0, 0xFFFF)
// DEFINE_IS_IN_UNICODE_BLOCK(LinearBSyllabary, 0x10000, 0x1007F)
// DEFINE_IS_IN_UNICODE_BLOCK(LinearBIdeograms, 0x10080, 0x100FF)
// DEFINE_IS_IN_UNICODE_BLOCK(AegeanNumbers, 0x10100, 0x1013F)
// DEFINE_IS_IN_UNICODE_BLOCK(AncientGreekNumbers, 0x10140, 0x1018F)
// DEFINE_IS_IN_UNICODE_BLOCK(AncientSymbols, 0x10190, 0x101CF)
// DEFINE_IS_IN_UNICODE_BLOCK(PhaistosDisc, 0x101D0, 0x101FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Lycian, 0x10280, 0x1029F)
// DEFINE_IS_IN_UNICODE_BLOCK(Carian, 0x102A0, 0x102DF)
// DEFINE_IS_IN_UNICODE_BLOCK(CopticEpactNumbers, 0x102E0, 0x102FF)
// DEFINE_IS_IN_UNICODE_BLOCK(OldItalic, 0x10300, 0x1032F)
// DEFINE_IS_IN_UNICODE_BLOCK(Gothic, 0x10330, 0x1034F)
// DEFINE_IS_IN_UNICODE_BLOCK(OldPermic, 0x10350, 0x1037F)
// DEFINE_IS_IN_UNICODE_BLOCK(Ugaritic, 0x10380, 0x1039F)
// DEFINE_IS_IN_UNICODE_BLOCK(OldPersian, 0x103A0, 0x103DF)
// DEFINE_IS_IN_UNICODE_BLOCK(Deseret, 0x10400, 0x1044F)
// DEFINE_IS_IN_UNICODE_BLOCK(Shavian, 0x10450, 0x1047F)
// DEFINE_IS_IN_UNICODE_BLOCK(Osmanya, 0x10480, 0x104AF)
// DEFINE_IS_IN_UNICODE_BLOCK(Osage, 0x104B0, 0x104FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Elbasan, 0x10500, 0x1052F)
// DEFINE_IS_IN_UNICODE_BLOCK(CaucasianAlbanian, 0x10530, 0x1056F)
// DEFINE_IS_IN_UNICODE_BLOCK(LinearA, 0x10600, 0x1077F)
// DEFINE_IS_IN_UNICODE_BLOCK(CypriotSyllabary, 0x10800, 0x1083F)
// DEFINE_IS_IN_UNICODE_BLOCK(ImperialAramaic, 0x10840, 0x1085F)
// DEFINE_IS_IN_UNICODE_BLOCK(Palmyrene, 0x10860, 0x1087F)
// DEFINE_IS_IN_UNICODE_BLOCK(Nabataean, 0x10880, 0x108AF)
// DEFINE_IS_IN_UNICODE_BLOCK(Hatran, 0x108E0, 0x108FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Phoenician, 0x10900, 0x1091F)
// DEFINE_IS_IN_UNICODE_BLOCK(Lydian, 0x10920, 0x1093F)
// DEFINE_IS_IN_UNICODE_BLOCK(MeroiticHieroglyphs, 0x10980, 0x1099F)
// DEFINE_IS_IN_UNICODE_BLOCK(MeroiticCursive, 0x109A0, 0x109FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Kharoshthi, 0x10A00, 0x10A5F)
// DEFINE_IS_IN_UNICODE_BLOCK(OldSouthArabian, 0x10A60, 0x10A7F)
// DEFINE_IS_IN_UNICODE_BLOCK(OldNorthArabian, 0x10A80, 0x10A9F)
// DEFINE_IS_IN_UNICODE_BLOCK(Manichaean, 0x10AC0, 0x10AFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Avestan, 0x10B00, 0x10B3F)
// DEFINE_IS_IN_UNICODE_BLOCK(InscriptionalParthian, 0x10B40, 0x10B5F)
// DEFINE_IS_IN_UNICODE_BLOCK(InscriptionalPahlavi, 0x10B60, 0x10B7F)
// DEFINE_IS_IN_UNICODE_BLOCK(PsalterPahlavi, 0x10B80, 0x10BAF)
// DEFINE_IS_IN_UNICODE_BLOCK(OldTurkic, 0x10C00, 0x10C4F)
// DEFINE_IS_IN_UNICODE_BLOCK(OldHungarian, 0x10C80, 0x10CFF)
// DEFINE_IS_IN_UNICODE_BLOCK(RumiNumeralSymbols, 0x10E60, 0x10E7F)
// DEFINE_IS_IN_UNICODE_BLOCK(Brahmi, 0x11000, 0x1107F)
// DEFINE_IS_IN_UNICODE_BLOCK(Kaithi, 0x11080, 0x110CF)
// DEFINE_IS_IN_UNICODE_BLOCK(SoraSompeng, 0x110D0, 0x110FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Chakma, 0x11100, 0x1114F)
// DEFINE_IS_IN_UNICODE_BLOCK(Mahajani, 0x11150, 0x1117F)
// DEFINE_IS_IN_UNICODE_BLOCK(Sharada, 0x11180, 0x111DF)
// DEFINE_IS_IN_UNICODE_BLOCK(SinhalaArchaicNumbers, 0x111E0, 0x111FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Khojki, 0x11200, 0x1124F)
// DEFINE_IS_IN_UNICODE_BLOCK(Multani, 0x11280, 0x112AF)
// DEFINE_IS_IN_UNICODE_BLOCK(Khudawadi, 0x112B0, 0x112FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Grantha, 0x11300, 0x1137F)
// DEFINE_IS_IN_UNICODE_BLOCK(Newa, 0x11400, 0x1147F)
// DEFINE_IS_IN_UNICODE_BLOCK(Tirhuta, 0x11480, 0x114DF)
// DEFINE_IS_IN_UNICODE_BLOCK(Siddham, 0x11580, 0x115FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Modi, 0x11600, 0x1165F)
// DEFINE_IS_IN_UNICODE_BLOCK(MongolianSupplement, 0x11660, 0x1167F)
// DEFINE_IS_IN_UNICODE_BLOCK(Takri, 0x11680, 0x116CF)
// DEFINE_IS_IN_UNICODE_BLOCK(Ahom, 0x11700, 0x1173F)
// DEFINE_IS_IN_UNICODE_BLOCK(WarangCiti, 0x118A0, 0x118FF)
// DEFINE_IS_IN_UNICODE_BLOCK(ZanabazarSquare, 0x11A00, 0x11A4F)
// DEFINE_IS_IN_UNICODE_BLOCK(Soyombo, 0x11A50, 0x11AAF)
// DEFINE_IS_IN_UNICODE_BLOCK(PauCinHau, 0x11AC0, 0x11AFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Bhaiksuki, 0x11C00, 0x11C6F)
// DEFINE_IS_IN_UNICODE_BLOCK(Marchen, 0x11C70, 0x11CBF)
// DEFINE_IS_IN_UNICODE_BLOCK(MasaramGondi, 0x11D00, 0x11D5F)
// DEFINE_IS_IN_UNICODE_BLOCK(Cuneiform, 0x12000, 0x123FF)
// DEFINE_IS_IN_UNICODE_BLOCK(CuneiformNumbersandPunctuation, 0x12400, 0x1247F)
// DEFINE_IS_IN_UNICODE_BLOCK(EarlyDynasticCuneiform, 0x12480, 0x1254F)
// DEFINE_IS_IN_UNICODE_BLOCK(EgyptianHieroglyphs, 0x13000, 0x1342F)
// DEFINE_IS_IN_UNICODE_BLOCK(AnatolianHieroglyphs, 0x14400, 0x1467F)
// DEFINE_IS_IN_UNICODE_BLOCK(BamumSupplement, 0x16800, 0x16A3F)
// DEFINE_IS_IN_UNICODE_BLOCK(Mro, 0x16A40, 0x16A6F)
// DEFINE_IS_IN_UNICODE_BLOCK(BassaVah, 0x16AD0, 0x16AFF)
// DEFINE_IS_IN_UNICODE_BLOCK(PahawhHmong, 0x16B00, 0x16B8F)
// DEFINE_IS_IN_UNICODE_BLOCK(Miao, 0x16F00, 0x16F9F)
// DEFINE_IS_IN_UNICODE_BLOCK(IdeographicSymbolsandPunctuation, 0x16FE0, 0x16FFF)
// DEFINE_IS_IN_UNICODE_BLOCK(Tangut, 0x17000, 0x187FF)
// DEFINE_IS_IN_UNICODE_BLOCK(TangutComponents, 0x18800, 0x18AFF)
// DEFINE_IS_IN_UNICODE_BLOCK(KanaSupplement, 0x1B000, 0x1B0FF)
// DEFINE_IS_IN_UNICODE_BLOCK(KanaExtendedA, 0x1B100, 0x1B12F)
// DEFINE_IS_IN_UNICODE_BLOCK(Nushu, 0x1B170, 0x1B2FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Duployan, 0x1BC00, 0x1BC9F)
// DEFINE_IS_IN_UNICODE_BLOCK(ShorthandFormatControls, 0x1BCA0, 0x1BCAF)
// DEFINE_IS_IN_UNICODE_BLOCK(ByzantineMusicalSymbols, 0x1D000, 0x1D0FF)
// DEFINE_IS_IN_UNICODE_BLOCK(MusicalSymbols, 0x1D100, 0x1D1FF)
// DEFINE_IS_IN_UNICODE_BLOCK(AncientGreekMusicalNotation, 0x1D200, 0x1D24F)
// DEFINE_IS_IN_UNICODE_BLOCK(TaiXuanJingSymbols, 0x1D300, 0x1D35F)
// DEFINE_IS_IN_UNICODE_BLOCK(CountingRodNumerals, 0x1D360, 0x1D37F)
// DEFINE_IS_IN_UNICODE_BLOCK(MathematicalAlphanumericSymbols, 0x1D400, 0x1D7FF)
// DEFINE_IS_IN_UNICODE_BLOCK(SuttonSignWriting, 0x1D800, 0x1DAAF)
// DEFINE_IS_IN_UNICODE_BLOCK(GlagoliticSupplement, 0x1E000, 0x1E02F)
// DEFINE_IS_IN_UNICODE_BLOCK(MendeKikakui, 0x1E800, 0x1E8DF)
// DEFINE_IS_IN_UNICODE_BLOCK(Adlam, 0x1E900, 0x1E95F)
// DEFINE_IS_IN_UNICODE_BLOCK(ArabicMathematicalAlphabeticSymbols, 0x1EE00, 0x1EEFF)
// DEFINE_IS_IN_UNICODE_BLOCK(MahjongTiles, 0x1F000, 0x1F02F)
// DEFINE_IS_IN_UNICODE_BLOCK(DominoTiles, 0x1F030, 0x1F09F)
// DEFINE_IS_IN_UNICODE_BLOCK(PlayingCards, 0x1F0A0, 0x1F0FF)
// DEFINE_IS_IN_UNICODE_BLOCK(EnclosedAlphanumericSupplement, 0x1F100, 0x1F1FF)
// DEFINE_IS_IN_UNICODE_BLOCK(EnclosedIdeographicSupplement, 0x1F200, 0x1F2FF)
// DEFINE_IS_IN_UNICODE_BLOCK(MiscellaneousSymbolsandPictographs, 0x1F300, 0x1F5FF)
// DEFINE_IS_IN_UNICODE_BLOCK(Emoticons, 0x1F600, 0x1F64F)
// DEFINE_IS_IN_UNICODE_BLOCK(OrnamentalDingbats, 0x1F650, 0x1F67F)
// DEFINE_IS_IN_UNICODE_BLOCK(TransportandMapSymbols, 0x1F680, 0x1F6FF)
// DEFINE_IS_IN_UNICODE_BLOCK(AlchemicalSymbols, 0x1F700, 0x1F77F)
// DEFINE_IS_IN_UNICODE_BLOCK(GeometricShapesExtended, 0x1F780, 0x1F7FF)
// DEFINE_IS_IN_UNICODE_BLOCK(SupplementalArrowsC, 0x1F800, 0x1F8FF)
// DEFINE_IS_IN_UNICODE_BLOCK(SupplementalSymbolsandPictographs, 0x1F900, 0x1F9FF)
// DEFINE_IS_IN_UNICODE_BLOCK(CJKUnifiedIdeographsExtensionB, 0x20000, 0x2A6DF)
// DEFINE_IS_IN_UNICODE_BLOCK(CJKUnifiedIdeographsExtensionC, 0x2A700, 0x2B73F)
// DEFINE_IS_IN_UNICODE_BLOCK(CJKUnifiedIdeographsExtensionD, 0x2B740, 0x2B81F)
// DEFINE_IS_IN_UNICODE_BLOCK(CJKUnifiedIdeographsExtensionE, 0x2B820, 0x2CEAF)
// DEFINE_IS_IN_UNICODE_BLOCK(CJKUnifiedIdeographsExtensionF, 0x2CEB0, 0x2EBEF)
// DEFINE_IS_IN_UNICODE_BLOCK(CJKCompatibilityIdeographsSupplement, 0x2F800, 0x2FA1F)
// DEFINE_IS_IN_UNICODE_BLOCK(Tags, 0xE0000, 0xE007F)
// DEFINE_IS_IN_UNICODE_BLOCK(VariationSelectorsSupplement, 0xE0100, 0xE01EF)
// DEFINE_IS_IN_UNICODE_BLOCK(SupplementaryPrivateUseAreaA, 0xF0000, 0xFFFFF)
// DEFINE_IS_IN_UNICODE_BLOCK(SupplementaryPrivateUseAreaB, 0x100000, 0x10FFFF)

const std::map<char16_t, char16_t> verticalPunctuation = {
    { u'!', u'︕' },  { u'#', u'＃' },  { u'$', u'＄' },  { u'%', u'％' },  { u'&', u'＆' },
    { u'(', u'︵' },  { u')', u'︶' },  { u'*', u'＊' },  { u'+', u'＋' },  { u',', u'︐' },
    { u'-', u'︲' },  { u'.', u'・' },  { u'/', u'／' },  { u':', u'︓' },  { u';', u'︔' },
    { u'<', u'︿' },  { u'=', u'＝' },  { u'>', u'﹀' },  { u'?', u'︖' },  { u'@', u'＠' },
    { u'[', u'﹇' },  { u'\\', u'＼' }, { u']', u'﹈' },  { u'^', u'＾' },  { u'_', u'︳' },
    { u'`', u'｀' },  { u'{', u'︷' },  { u'|', u'―' },   { u'}', u'︸' },  { u'~', u'～' },
    { u'¢', u'￠' },  { u'£', u'￡' },  { u'¥', u'￥' },  { u'¦', u'￤' },  { u'¬', u'￢' },
    { u'¯', u'￣' },  { u'–', u'︲' },  { u'—', u'︱' },  { u'‘', u'﹃' },  { u'’', u'﹄' },
    { u'“', u'﹁' },  { u'”', u'﹂' },  { u'…', u'︙' },  { u'‧', u'・' },  { u'₩', u'￦' },
    { u'、', u'︑' }, { u'。', u'︒' }, { u'〈', u'︿' }, { u'〉', u'﹀' }, { u'《', u'︽' },
    { u'》', u'︾' }, { u'「', u'﹁' }, { u'」', u'﹂' }, { u'『', u'﹃' }, { u'』', u'﹄' },
    { u'【', u'︻' }, { u'】', u'︼' }, { u'〔', u'︹' }, { u'〕', u'︺' }, { u'〖', u'︗' },
    { u'〗', u'︘' }, { u'！', u'︕' }, { u'（', u'︵' }, { u'）', u'︶' }, { u'，', u'︐' },
    { u'－', u'︲' }, { u'．', u'・' }, { u'：', u'︓' }, { u'；', u'︔' }, { u'＜', u'︿' },
    { u'＞', u'﹀' }, { u'？', u'︖' }, { u'［', u'﹇' }, { u'］', u'﹈' }, { u'＿', u'︳' },
    { u'｛', u'︷' }, { u'｜', u'―' },  { u'｝', u'︸' }, { u'｟', u'︵' }, { u'｠', u'︶' },
    { u'｡', u'︒' },  { u'｢', u'﹁' },  { u'｣', u'﹂' },
};
} // namespace

namespace mbgl {
namespace util {
namespace i18n {

bool allowsWordBreaking(char16_t chr) {
    return (chr == 0x0a      /* newline */
            || chr == 0x20   /* space */
            || chr == 0x26   /* ampersand */
            || chr == 0x28   /* open parenthesis */
            || chr == 0x29   /* close parenthesis */
            || chr == 0x2b   /* plus sign */
            || chr == 0x2d   /* hyphen-minus */
            || chr == 0x2f   /* solidus */
            || chr == 0xad   /* soft hyphen */
            || chr == 0xb7   /* middle dot */
            || chr == 0x200b /* zero-width space */
            || chr == 0x2010 /* hyphen */
            || chr == 0x2013 /* en dash */);
}

bool charAllowsLetterSpacing(char16_t chr) {
    return !(isInArabic(chr) || isInArabicSupplement(chr) || isInArabicExtendedA(chr) || isInArabicPresentationFormsA(chr) || isInArabicPresentationFormsB(chr));
}

bool allowsLetterSpacing(const std::u16string& string) {
    return std::all_of(string.begin(), string.end(), charAllowsLetterSpacing);
}

bool allowsIdeographicBreaking(const std::u16string& string) {
    for (char16_t chr : string) {
        if (!allowsIdeographicBreaking(chr)) {
            return false;
        }
    }
    return true;
}

bool allowsIdeographicBreaking(char16_t chr) {
    // Allow U+2027 "Interpunct" for hyphenation of Chinese words
    if (chr == 0x2027)
        return true;

    // Return early for characters outside all ideographic ranges.
    if (chr < 0x2E80)
        return false;

    return (isInBopomofo(chr) || isInBopomofoExtended(chr) || isInCJKCompatibility(chr) ||
            isInCJKCompatibilityForms(chr) || isInCJKCompatibilityIdeographs(chr) ||
            isInCJKRadicalsSupplement(chr) || isInCJKStrokes(chr) ||
            isInCJKSymbolsandPunctuation(chr) || isInCJKUnifiedIdeographs(chr) ||
            isInCJKUnifiedIdeographsExtensionA(chr) || isInEnclosedCJKLettersandMonths(chr) ||
            isInHalfwidthandFullwidthForms(chr) || isInHiragana(chr) ||
            isInIdeographicDescriptionCharacters(chr) || isInKangxiRadicals(chr) ||
            isInKatakana(chr) || isInKatakanaPhoneticExtensions(chr) || isInVerticalForms(chr) ||
            isInYiRadicals(chr) || isInYiSyllables(chr));

    // The following blocks also allow ideographic breaking; however, for other
    // reasons, Mapbox GL lacks support for codepoints beyond U+FFFF.
    // https://github.com/mapbox/mapbox-gl/issues/29
    // return (isInTangut(chr)
    //        || isInTangutComponents(chr)
    //        || isInIdeographicSymbolsandPunctuation(chr)
    //        || isInNushu(chr)
    //        || isInEnclosedIdeographicSupplement(chr)
    //        || isInCJKUnifiedIdeographsExtensionB(chr)
    //        || isInCJKUnifiedIdeographsExtensionC(chr)
    //        || isInCJKUnifiedIdeographsExtensionD(chr)
    //        || isInCJKUnifiedIdeographsExtensionE(chr)
    //        || isInCJKUnifiedIdeographsExtensionF(chr)
    //        || isInCJKCompatibilityIdeographsSupplement(chr));
}

bool allowsFixedWidthGlyphGeneration(char16_t chr) {
    // Mirrors conservative set of characters used in glyph_manager.js/_tinySDF
    return isInCJKUnifiedIdeographs(chr) || isInHangulSyllables(chr);
}

bool allowsVerticalWritingMode(const std::u16string& string) {
    for (char32_t chr : string) {
        if (hasUprightVerticalOrientation(chr)) {
            return true;
        }
    }
    return false;
}

// The following logic comes from
// <http://www.unicode.org/Public/vertical/revision-17/VerticalOrientation-17.txt>.
// The data file denotes with “U” or “Tu” any codepoint that may be drawn
// upright in vertical text but does not distinguish between upright and
// “neutral” characters.

bool hasUprightVerticalOrientation(char16_t chr) {
    if (chr == u'˪' || chr == u'˫')
        return true;

    // Return early for characters outside all ranges whose characters remain
    // upright in vertical writing mode.
    if (chr < 0x1100)
        return false;

    if (isInBopomofo(chr) || isInBopomofoExtended(chr))
        return true;
    if (isInCJKCompatibilityForms(chr)) {
        if (!(chr >= u'﹉' && chr <= u'﹏'))
            return true;
    }
    if (isInCJKCompatibility(chr) || isInCJKCompatibilityIdeographs(chr) ||
        isInCJKRadicalsSupplement(chr) || isInCJKStrokes(chr))
        return true;
    if (isInCJKSymbolsandPunctuation(chr)) {
        if (!(chr >= u'〈' && chr <= u'】') && !(chr >= u'〔' && chr <= u'〟') && chr != u'〰')
            return true;
    }
    if (isInCJKUnifiedIdeographs(chr) || isInCJKUnifiedIdeographsExtensionA(chr) ||
        isInEnclosedCJKLettersandMonths(chr) || isInHangulCompatibilityJamo(chr) ||
        isInHangulJamo(chr) || isInHangulJamoExtendedA(chr) || isInHangulJamoExtendedB(chr) ||
        isInHangulSyllables(chr) || isInHiragana(chr) ||
        isInIdeographicDescriptionCharacters(chr) || isInKanbun(chr) || isInKangxiRadicals(chr))
        return true;
    if (isInKatakana(chr)) {
        if (chr != u'ー')
            return true;
    }
    if (isInKatakanaPhoneticExtensions(chr))
        return true;
    if (isInHalfwidthandFullwidthForms(chr)) {
        if (chr != u'（' && chr != u'）' && chr != u'－' && !(chr >= u'：' && chr <= u'＞') &&
            chr != u'［' && chr != u'］' && chr != u'＿' && !(chr >= u'｛' && chr <= 0xFFDF) &&
            chr != u'￣' && !(chr >= u'￨' && chr <= 0xFFEF))
            return true;
    }
    if (isInSmallFormVariants(chr)) {
        if (!(chr >= u'﹘' && chr <= u'﹞') && !(chr >= u'﹣' && chr <= u'﹦'))
            return true;
    }
    if (isInUnifiedCanadianAboriginalSyllabics(chr) ||
        isInUnifiedCanadianAboriginalSyllabicsExtended(chr) || isInVerticalForms(chr) ||
        isInYijingHexagramSymbols(chr) || isInYiSyllables(chr) || isInYiRadicals(chr))
        return true;

    // https://github.com/mapbox/mapbox-gl/issues/29

    // if (isInMeroiticHieroglyphs(chr)) return true;
    // if (isInSiddham(chr)) return true;
    // if (isInEgyptianHieroglyphs(chr)) return true;
    // if (isInAnatolianHieroglyphs(chr)) return true;
    // if (isInIdeographicSymbolsandPunctuation(chr)) return true;
    // if (isInTangut(chr)) return true;
    // if (isInTangutComponents(chr)) return true;
    // if (isInKanaSupplement(chr)) return true;
    // if (isInKanaExtendedA(chr)) return true;
    // if (isInNushu(chr)) return true;
    // if (isInByzantineMusicalSymbols(chr)) return true;
    // if (isInMusicalSymbols(chr)) return true;
    // if (isInTaiXuanJingSymbols(chr)) return true;
    // if (isInCountingRodNumerals(chr)) return true;
    // if (isInSuttonSignWriting(chr)) return true;
    // if (isInMahjongTiles(chr)) return true;
    // if (isInDominoTiles(chr)) return true;
    // if (isInPlayingCards(chr)) return true;
    // if (isInEnclosedAlphanumericSupplement(chr)) return true;
    // if (isInEnclosedIdeographicSupplement(chr)) return true;
    // if (isInMiscellaneousSymbolsandPictographs(chr)) return true;
    // if (isInEmoticons(chr)) return true;
    // if (isInOrnamentalDingbats(chr)) return true;
    // if (isInTransportandMapSymbols(chr)) return true;
    // if (isInAlchemicalSymbols(chr)) return true;
    // if (isInGeometricShapesExtended(chr)) return true;
    // if (isInSupplementalSymbolsandPictographs(chr)) return true;
    // if (isInCJKUnifiedIdeographsExtensionB(chr)) return true;
    // if (isInCJKUnifiedIdeographsExtensionC(chr)) return true;
    // if (isInCJKUnifiedIdeographsExtensionD(chr)) return true;
    // if (isInCJKUnifiedIdeographsExtensionE(chr)) return true;
    // if (isInCJKUnifiedIdeographsExtensionF(chr)) return true;
    // if (isInCJKCompatibilityIdeographsSupplement(chr)) return true;

    return false;
}

bool hasNeutralVerticalOrientation(char16_t chr) {
    if (isInLatin1Supplement(chr)) {
        if (chr == u'§' || chr == u'©' || chr == u'®' || chr == u'±' || chr == u'¼' ||
            chr == u'½' || chr == u'¾' || chr == u'×' || chr == u'÷') {
            return true;
        }
    }
    if (isInGeneralPunctuation(chr)) {
        if (chr == u'‖' || chr == u'†' || chr == u'‡' || chr == u'‰' || chr == u'‱' ||
            chr == u'※' || chr == u'‼' || chr == u'⁂' || chr == u'⁇' || chr == u'⁈' ||
            chr == u'⁉' || chr == u'⁑') {
            return true;
        }
    }
    if (isInLetterlikeSymbols(chr) || isInNumberForms(chr)) {
        return true;
    }
    if (isInMiscellaneousTechnical(chr)) {
        if ((chr >= u'⌀' && chr <= u'⌇') || (chr >= u'⌌' && chr <= u'⌟') ||
            (chr >= u'⌤' && chr <= u'⌨') || chr == u'⌫' || (chr >= u'⍽' && chr <= u'⎚') ||
            (chr >= u'⎾' && chr <= u'⏍') || chr == u'⏏' || (chr >= u'⏑' && chr <= u'⏛') ||
            (chr >= u'⏢' && chr <= 0x23FF)) {
            return true;
        }
    }
    if (isInControlPictures(chr) || isInOpticalCharacterRecognition(chr) ||
        isInEnclosedAlphanumerics(chr) || isInGeometricShapes(chr)) {
        return true;
    }
    if (isInMiscellaneousSymbols(chr)) {
        if ((chr >= u'⬒' && chr <= u'⬯') ||
            (chr >= u'⭐' && chr <= 0x2B59 /* heavy circled saltire */) ||
            (chr >= 0x2BB8 /* upwards white arrow from bar with horizontal bar */ &&
             chr <= 0x2BEB)) {
            return true;
        }
    }
    if (isInCJKSymbolsandPunctuation(chr) || isInKatakana(chr) || isInPrivateUseArea(chr) ||
        isInCJKCompatibilityForms(chr) || isInSmallFormVariants(chr) ||
        isInHalfwidthandFullwidthForms(chr)) {
        return true;
    }
    if (chr == u'∞' || chr == u'∴' || chr == u'∵' ||
        (chr >= 0x2700 /* black safety scissors */ && chr <= u'❧') ||
        (chr >= u'❶' && chr <= u'➓') || chr == 0xFFFC /* object replacement character */ ||
        chr == 0xFFFD /* replacement character */) {
        return true;
    }
    return false;
}

bool hasRotatedVerticalOrientation(char16_t chr) {
    return !(hasUprightVerticalOrientation(chr) || hasNeutralVerticalOrientation(chr));
}

std::u16string verticalizePunctuation(const std::u16string& input) {
    std::u16string output;

    for (size_t i = 0; i < input.size(); i++) {
        char16_t nextCharCode = i < input.size() - 1 ? input[i + 1] : 0;
        char16_t prevCharCode = i ? input[i - 1] : 0;

        bool canReplacePunctuation =
            ((!nextCharCode || !hasRotatedVerticalOrientation(nextCharCode) ||
              verticalPunctuation.count(input[i + 1])) &&
             (!prevCharCode || !hasRotatedVerticalOrientation(prevCharCode) ||
              verticalPunctuation.count(input[i - 1])));

        if (char16_t repl = canReplacePunctuation ? verticalizePunctuation(input[i]) : 0) {
            output += repl;
        } else {
            output += input[i];
        }
    }

    return output;
}

char16_t verticalizePunctuation(char16_t chr) {
    return verticalPunctuation.count(chr) ? verticalPunctuation.at(chr) : 0;
}

} // namespace i18n
} // namespace util
} // namespace mbgl
