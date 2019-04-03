#include <mbgl/text/bidi.hpp>
#include <mbgl/util/traits.hpp>

#include <unicode/ubidi.h>
#include <unicode/ushape.h>

#include <memory>

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
    
std::vector<StyledText> BiDi::processStyledText(const StyledText& input, std::set<std::size_t> lineBreakPoints) {
    std::vector<StyledText> lines;
    const auto& inputText = input.first;
    const auto& styleIndices = input.second;
    
    UErrorCode errorCode = U_ZERO_ERROR;
    
    ubidi_setPara(impl->bidiText, mbgl::utf16char_cast<const UChar*>(inputText.c_str()), static_cast<int32_t>(inputText.size()),
                  UBIDI_DEFAULT_LTR, nullptr, &errorCode);
    
    if (U_FAILURE(errorCode)) {
        throw std::runtime_error(std::string("BiDi::processStyledText: ") + u_errorName(errorCode));
    }
    
    mergeParagraphLineBreaks(lineBreakPoints);
    
    std::size_t lineStartIndex = 0;
    
    for (std::size_t lineBreakPoint : lineBreakPoints) {
        StyledText line;
        line.second.reserve(lineBreakPoint - lineStartIndex);

        errorCode = U_ZERO_ERROR;
        ubidi_setLine(impl->bidiText, static_cast<int32_t>(lineStartIndex), static_cast<int32_t>(lineBreakPoint), impl->bidiLine, &errorCode);
        if (U_FAILURE(errorCode)) {
            throw std::runtime_error(std::string("BiDi::processStyledText (setLine): ") + u_errorName(errorCode));
        }
        
        errorCode = U_ZERO_ERROR;
        uint32_t runCount = ubidi_countRuns(impl->bidiLine, &errorCode);
        if (U_FAILURE(errorCode)) {
            throw std::runtime_error(std::string("BiDi::processStyledText (countRuns): ") + u_errorName(errorCode));
        }
        
        for (uint32_t runIndex = 0; runIndex < runCount; runIndex++) {
            int32_t runLogicalStart;
            int32_t runLength;
            UBiDiDirection direction = ubidi_getVisualRun(impl->bidiLine, runIndex, &runLogicalStart, &runLength);
            const bool isReversed = direction == UBIDI_RTL;
            
            std::size_t logicalStart = lineStartIndex + runLogicalStart;
            std::size_t logicalEnd = logicalStart + runLength;
            if (isReversed) {
                // Within this reversed section, iterate logically backwards
                // Each time we see a change in style, render a reversed chunk
                // of everything since the last change
                std::size_t styleRunStart = logicalEnd;
                uint8_t currentStyleIndex = styleIndices.at(styleRunStart - 1);
                for (std::size_t i = logicalEnd - 1; i >= logicalStart; i--) {
                    if (currentStyleIndex != styleIndices.at(i) || i == logicalStart) {
                        std::size_t styleRunEnd = i == logicalStart ? i : i + 1;
                        std::u16string reversed = writeReverse(inputText, styleRunEnd, styleRunStart);
                        line.first += reversed;
                        for (std::size_t j = 0; j < reversed.size(); j++) {
                            line.second.push_back(currentStyleIndex);
                        }
                        currentStyleIndex = styleIndices.at(i);
                        styleRunStart = styleRunEnd;
                    }
                    if (i == 0) {
                        break;
                    }
                }
                
            } else {
                line.first += input.first.substr(logicalStart, runLength);
                line.second.insert(line.second.end(), styleIndices.begin() + logicalStart, styleIndices.begin() + logicalStart + runLength);
            }
        }

        lines.push_back(line);
        lineStartIndex = lineBreakPoint;
    }

    return lines;
}
    
std::u16string BiDi::writeReverse(const std::u16string& input, std::size_t logicalStart, std::size_t logicalEnd) {
    UErrorCode errorCode = U_ZERO_ERROR;
    auto logicalLength = static_cast<int32_t>(logicalEnd - logicalStart);
    std::u16string outputText(logicalLength + 1, 0);
    
    // UBIDI_DO_MIRRORING: Apply unicode mirroring of characters like parentheses
    // UBIDI_REMOVE_BIDI_CONTROLS: Now that all the lines are set, remove control characters so that
    // they don't show up on screen (some fonts have glyphs representing them)
    int32_t outputLength =
        ubidi_writeReverse(mbgl::utf16char_cast<const UChar*>(&input[logicalStart]),
                           logicalLength,
                           mbgl::utf16char_cast<UChar*>(&outputText[0]),
                           logicalLength + 1, // Extra room for null terminator, although we don't really need to have ICU write it for us
                           UBIDI_DO_MIRRORING | UBIDI_REMOVE_BIDI_CONTROLS,
                           &errorCode);
    
    if (U_FAILURE(errorCode)) {
        throw std::runtime_error(std::string("BiDi::writeReverse: ") + u_errorName(errorCode));
    }
    
    outputText.resize(outputLength); // REMOVE_BIDI_CONTROLS may have shrunk the string

    return outputText;
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
    int32_t finalLength = ubidi_writeReordered(impl->bidiLine,
                                               mbgl::utf16char_cast<UChar*>(&outputText[0]),
                                               outputLength,
                                               UBIDI_DO_MIRRORING | UBIDI_REMOVE_BIDI_CONTROLS,
                                               &errorCode);

    outputText.resize(finalLength); // REMOVE_BIDI_CONTROLS may have shrunk the string

    if (U_FAILURE(errorCode)) {
        throw std::runtime_error(std::string("BiDi::getLine (writeReordered): ") +
                                 u_errorName(errorCode));
    }

    return outputText;
}

} // end namespace mbgl
