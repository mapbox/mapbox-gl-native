#include <mbgl/text/bidi.hpp>
#include <mbgl/util/traits.hpp>

#include <unicode/ubidi.h>
#include <unicode/ushape.h>

#include <memory>
#include <stdexcept>

namespace mbgl {

class BiDiImpl {
public:
    BiDiImpl() : bidiText(ubidi_open()), bidiLine(ubidi_open()) {
    }
    ~BiDiImpl() {
        ubidi_close(bidiText);
        ubidi_close(bidiLine);
    }

    UBiDi* bidiText = nullptr;
    UBiDi* bidiLine = nullptr;
};

BiDi::BiDi() : impl(std::make_unique<BiDiImpl>()) {}
BiDi::~BiDi() = default;

// Takes UTF16 input in logical order and applies Arabic shaping to the input while maintaining
// logical order. Output won't be intelligible until the bidirectional algorithm is applied
std::u16string applyArabicShaping(const std::u16string& input) {
    UErrorCode errorCode = U_ZERO_ERROR;

    const int32_t outputLength =
        u_shapeArabic(mbgl::utf16char_cast<const UChar*>(input.c_str()), static_cast<int32_t>(input.size()), nullptr, 0,
                      (U_SHAPE_LETTERS_SHAPE & U_SHAPE_LETTERS_MASK) |
                          (U_SHAPE_TEXT_DIRECTION_LOGICAL & U_SHAPE_TEXT_DIRECTION_MASK),
                      &errorCode);

    // Pre-flighting will always set U_BUFFER_OVERFLOW_ERROR
    errorCode = U_ZERO_ERROR;

    std::u16string outputText(outputLength, 0);

    u_shapeArabic(mbgl::utf16char_cast<const UChar*>(input.c_str()), static_cast<int32_t>(input.size()), mbgl::utf16char_cast<UChar*>(&outputText[0]), outputLength,
                  (U_SHAPE_LETTERS_SHAPE & U_SHAPE_LETTERS_MASK) |
                      (U_SHAPE_TEXT_DIRECTION_LOGICAL & U_SHAPE_TEXT_DIRECTION_MASK),
                  &errorCode);

    // If the algorithm fails for any reason, fall back to non-transformed text
    if (U_FAILURE(errorCode))
        return input;

    return outputText;
}

void BiDi::mergeParagraphLineBreaks(std::set<size_t>& lineBreakPoints) {
    int32_t paragraphCount = ubidi_countParagraphs(impl->bidiText);
    for (int32_t i = 0; i < paragraphCount; i++) {
        UErrorCode errorCode = U_ZERO_ERROR;
        int32_t paragraphEndIndex;
        ubidi_getParagraphByIndex(impl->bidiText, i, nullptr, &paragraphEndIndex, nullptr, &errorCode);

        if (U_FAILURE(errorCode)) {
            throw std::runtime_error(std::string("ProcessedBiDiText::mergeParagraphLineBreaks: ") +
                                     u_errorName(errorCode));
        }

        lineBreakPoints.insert(static_cast<std::size_t>(paragraphEndIndex));
    }
}

std::vector<std::u16string> BiDi::applyLineBreaking(std::set<std::size_t> lineBreakPoints) {
    // BiDi::getLine will error if called across a paragraph boundary, so we need to ensure that all
    // paragraph boundaries are included in the set of line break points. The calling code might not
    // include the line break because it didn't need to wrap at that point, or because the text was
    // separated with a more exotic code point such as (U+001C)
    mergeParagraphLineBreaks(lineBreakPoints);

    std::vector<std::u16string> transformedLines;
    transformedLines.reserve(lineBreakPoints.size());

    std::size_t start = 0;
    for (std::size_t lineBreakPoint : lineBreakPoints) {
        transformedLines.push_back(getLine(start, lineBreakPoint));
        start = lineBreakPoint;
    }

    return transformedLines;
}

std::vector<std::u16string> BiDi::processText(const std::u16string& input,
                                              std::set<std::size_t> lineBreakPoints) {
    UErrorCode errorCode = U_ZERO_ERROR;

    ubidi_setPara(impl->bidiText, mbgl::utf16char_cast<const UChar*>(input.c_str()), static_cast<int32_t>(input.size()),
                  UBIDI_DEFAULT_LTR, nullptr, &errorCode);

    if (U_FAILURE(errorCode)) {
        throw std::runtime_error(std::string("BiDi::processText: ") + u_errorName(errorCode));
    }

    return applyLineBreaking(lineBreakPoints);
}

std::u16string BiDi::getLine(std::size_t start, std::size_t end) {
    UErrorCode errorCode = U_ZERO_ERROR;
    ubidi_setLine(impl->bidiText, static_cast<int32_t>(start), static_cast<int32_t>(end), impl->bidiLine, &errorCode);

    if (U_FAILURE(errorCode)) {
        throw std::runtime_error(std::string("BiDi::getLine (setLine): ") + u_errorName(errorCode));
    }

    // Because we set UBIDI_REMOVE_BIDI_CONTROLS, the output may be smaller than what we reserve
    //  Setting UBIDI_INSERT_LRM_FOR_NUMERIC would require
    //  ubidi_getLength(pBiDi)+2*ubidi_countRuns(pBiDi)
    const int32_t outputLength = ubidi_getProcessedLength(impl->bidiLine);
    std::u16string outputText(outputLength, 0);

    // UBIDI_DO_MIRRORING: Apply unicode mirroring of characters like parentheses
    // UBIDI_REMOVE_BIDI_CONTROLS: Now that all the lines are set, remove control characters so that
    // they don't show up on screen (some fonts have glyphs representing them)
    ubidi_writeReordered(impl->bidiLine, mbgl::utf16char_cast<UChar*>(&outputText[0]), outputLength,
                         UBIDI_DO_MIRRORING | UBIDI_REMOVE_BIDI_CONTROLS, &errorCode);

    if (U_FAILURE(errorCode)) {
        throw std::runtime_error(std::string("BiDi::getLine (writeReordered): ") +
                                 u_errorName(errorCode));
    }

    return outputText;
}

} // end namespace mbgl
