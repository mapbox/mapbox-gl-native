#include <mbgl/text/shaping.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/layout/symbol_feature.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/text/bidi.hpp>

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <cmath>

namespace mbgl {

struct AnchorAlignment {
    AnchorAlignment(float horizontal_, float vertical_)
        : horizontalAlign(horizontal_), verticalAlign(vertical_) {
    }

    float horizontalAlign;
    float verticalAlign;
};

AnchorAlignment getAnchorAlignment(style::SymbolAnchorType anchor) {
    float horizontalAlign = 0.5;
    float verticalAlign = 0.5;

    switch (anchor) {
    case style::SymbolAnchorType::Top:
    case style::SymbolAnchorType::Bottom:
    case style::SymbolAnchorType::Center:
        break;
    case style::SymbolAnchorType::Right:
    case style::SymbolAnchorType::TopRight:
    case style::SymbolAnchorType::BottomRight:
        horizontalAlign = 1;
        break;
    case style::SymbolAnchorType::Left:
    case style::SymbolAnchorType::TopLeft:
    case style::SymbolAnchorType::BottomLeft:
        horizontalAlign = 0;
        break;
    }

    switch (anchor) {
    case style::SymbolAnchorType::Left:
    case style::SymbolAnchorType::Right:
    case style::SymbolAnchorType::Center:
        break;
    case style::SymbolAnchorType::Bottom:
    case style::SymbolAnchorType::BottomLeft:
    case style::SymbolAnchorType::BottomRight:
        verticalAlign = 1;
        break;
    case style::SymbolAnchorType::Top:
    case style::SymbolAnchorType::TopLeft:
    case style::SymbolAnchorType::TopRight:
        verticalAlign = 0;
        break;
    }

    return AnchorAlignment(horizontalAlign, verticalAlign);
}

PositionedIcon PositionedIcon::shapeIcon(const ImagePosition& image, const std::array<float, 2>& iconOffset, style::SymbolAnchorType iconAnchor, const float iconRotation) {
    AnchorAlignment anchorAlign = getAnchorAlignment(iconAnchor);
    float dx = iconOffset[0];
    float dy = iconOffset[1];
    float x1 = dx - image.displaySize()[0] * anchorAlign.horizontalAlign;
    float x2 = x1 + image.displaySize()[0];
    float y1 = dy - image.displaySize()[1] * anchorAlign.verticalAlign;
    float y2 = y1 + image.displaySize()[1];

    return PositionedIcon { image, y1, y2, x1, x2, iconRotation };
}

void align(Shaping& shaping,
           const float justify,
           const float horizontalAlign,
           const float verticalAlign,
           const float maxLineLength,
           const float lineHeight,
           const std::size_t lineCount) {
    const float shiftX = (justify - horizontalAlign) * maxLineLength;
    const float shiftY = (-verticalAlign * lineCount + 0.5) * lineHeight;
    
    for (auto& glyph : shaping.positionedGlyphs) {
        glyph.x += shiftX;
        glyph.y += shiftY;
    }
}

// justify left = 0, right = 1, center = .5
void justifyLine(std::vector<PositionedGlyph>& positionedGlyphs,
                 const Glyphs& glyphs,
                 std::size_t start,
                 std::size_t end,
                 float justify) {
    if (!justify) {
        return;
    }
    
    PositionedGlyph& glyph = positionedGlyphs[end];
    auto it = glyphs.find(glyph.glyph);
    if (it != glyphs.end() && it->second) {
        const uint32_t lastAdvance = (*it->second)->metrics.advance;
        const float lineIndent = float(glyph.x + lastAdvance) * justify;
        
        for (std::size_t j = start; j <= end; j++) {
            positionedGlyphs[j].x -= lineIndent;
        }
    }
}

float determineAverageLineWidth(const std::u16string& logicalInput,
                                const float spacing,
                                float maxWidth,
                                const Glyphs& glyphs) {
    float totalWidth = 0;
    
    for (char16_t chr : logicalInput) {
        auto it = glyphs.find(chr);
        if (it != glyphs.end() && it->second) {
            totalWidth += (*it->second)->metrics.advance + spacing;
        }
    }
    
    int32_t targetLineCount = ::fmax(1, std::ceil(totalWidth / maxWidth));
    return totalWidth / targetLineCount;
}

float calculateBadness(const float lineWidth, const float targetWidth, const float penalty, const bool isLastBreak) {
    const float raggedness = std::pow(lineWidth - targetWidth, 2);
    if (isLastBreak) {
        // Favor finals lines shorter than average over longer than average
        if (lineWidth < targetWidth) {
            return raggedness / 2;
        } else {
            return raggedness * 2;
        }
    }
    if (penalty < 0) {
        return raggedness - std::pow(penalty, 2);
    }
    return raggedness + std::pow(penalty, 2);
}

float calculatePenalty(char16_t codePoint, char16_t nextCodePoint) {
    float penalty = 0;
    // Force break on newline
    if (codePoint == 0x0a) {
        penalty -= 10000;
    }
    // Penalize open parenthesis at end of line
    if (codePoint == 0x28 || codePoint == 0xff08) {
        penalty += 50;
    }
    
    // Penalize close parenthesis at beginning of line
    if (nextCodePoint == 0x29 || nextCodePoint == 0xff09) {
        penalty += 50;
    }
    
    return penalty;
}

struct PotentialBreak {
    PotentialBreak(const std::size_t p_index, const float p_x, const PotentialBreak* p_priorBreak, const float p_badness)
    : index(p_index), x(p_x), priorBreak(p_priorBreak), badness(p_badness)
    {}
    
    const std::size_t index;
    const float x;
    const PotentialBreak* priorBreak;
    const float badness;
};


PotentialBreak evaluateBreak(const std::size_t breakIndex, const float breakX, const float targetWidth, const std::list<PotentialBreak>& potentialBreaks, const float penalty, const bool isLastBreak) {
    // We could skip evaluating breaks where the line length (breakX - priorBreak.x) > maxWidth
    //  ...but in fact we allow lines longer than maxWidth (if there's no break points)
    //  ...and when targetWidth and maxWidth are close, strictly enforcing maxWidth can give
    //     more lopsided results.
    
    const PotentialBreak* bestPriorBreak = nullptr;
    float bestBreakBadness = calculateBadness(breakX, targetWidth, penalty, isLastBreak);
    for (const auto& potentialBreak : potentialBreaks) {
        const float lineWidth = breakX - potentialBreak.x;
        float breakBadness =
        calculateBadness(lineWidth, targetWidth, penalty, isLastBreak) + potentialBreak.badness;
        if (breakBadness <= bestBreakBadness) {
            bestPriorBreak = &potentialBreak;
            bestBreakBadness = breakBadness;
        }
    }
    
    return PotentialBreak(breakIndex, breakX, bestPriorBreak, bestBreakBadness);
}

std::set<std::size_t> leastBadBreaks(const PotentialBreak& lastLineBreak) {
    std::set<std::size_t> leastBadBreaks = { lastLineBreak.index };
    const PotentialBreak* priorBreak = lastLineBreak.priorBreak;
    while (priorBreak) {
        leastBadBreaks.insert(priorBreak->index);
        priorBreak = priorBreak->priorBreak;
    }
    return leastBadBreaks;
}


// We determine line breaks based on shaped text in logical order. Working in visual order would be
//  more intuitive, but we can't do that because the visual order may be changed by line breaks!
std::set<std::size_t> determineLineBreaks(const std::u16string& logicalInput,
                                          const float spacing,
                                          float maxWidth,
                                          const WritingModeType writingMode,
                                          const Glyphs& glyphs) {
    if (!maxWidth || writingMode != WritingModeType::Horizontal) {
        return {};
    }
    
    if (logicalInput.empty()) {
        return {};
    }
    
    const float targetWidth = determineAverageLineWidth(logicalInput, spacing, maxWidth, glyphs);
    
    std::list<PotentialBreak> potentialBreaks;
    float currentX = 0;
    
    for (std::size_t i = 0; i < logicalInput.size(); i++) {
        const char16_t codePoint = logicalInput[i];
        auto it = glyphs.find(codePoint);
        if (it != glyphs.end() && it->second && !boost::algorithm::is_any_of(u" \t\n\v\f\r")(codePoint)) {
            currentX += (*it->second)->metrics.advance + spacing;
        }
        
        // Ideographic characters, spaces, and word-breaking punctuation that often appear without
        // surrounding spaces.
        if ((i < logicalInput.size() - 1) &&
            (util::i18n::allowsWordBreaking(codePoint) || util::i18n::allowsIdeographicBreaking(codePoint))) {
            potentialBreaks.push_back(evaluateBreak(i+1, currentX, targetWidth, potentialBreaks,
                                                    calculatePenalty(codePoint, logicalInput[i+1]),
                                                    false));
        }
    }
    
    return leastBadBreaks(evaluateBreak(logicalInput.size(), currentX, targetWidth, potentialBreaks, 0, true));
}

void shapeLines(Shaping& shaping,
                          const std::vector<std::u16string>& lines,
                          const float spacing,
                          const float lineHeight,
                          const style::SymbolAnchorType textAnchor,
                          const style::TextJustifyType textJustify,
                          const float verticalHeight,
                          const WritingModeType writingMode,
                          const Glyphs& glyphs) {
    
    // the y offset *should* be part of the font metadata
    const int32_t yOffset = -17;
    
    float x = 0;
    float y = yOffset;
    
    float maxLineLength = 0;

    const float justify = textJustify == style::TextJustifyType::Right ? 1 :
        textJustify == style::TextJustifyType::Left ? 0 :
        0.5;
    
    for (std::u16string line : lines) {
        // Collapse whitespace so it doesn't throw off justification
        boost::algorithm::trim_if(line, boost::algorithm::is_any_of(u" \t\n\v\f\r"));
        
        if (line.empty()) {
            y += lineHeight; // Still need a line feed after empty line
            continue;
        }
        
        std::size_t lineStartIndex = shaping.positionedGlyphs.size();
        for (char16_t chr : line) {
            auto it = glyphs.find(chr);
            if (it == glyphs.end() || !it->second) {
                continue;
            }
            
            const Glyph& glyph = **it->second;
            
            if (writingMode == WritingModeType::Horizontal || !util::i18n::hasUprightVerticalOrientation(chr)) {
                shaping.positionedGlyphs.emplace_back(chr, x, y, false);
                x += glyph.metrics.advance + spacing;
            } else {
                shaping.positionedGlyphs.emplace_back(chr, x, 0, true);
                x += verticalHeight + spacing;
            }
        }
        
        // Only justify if we placed at least one glyph
        if (shaping.positionedGlyphs.size() != lineStartIndex) {
            float lineLength = x - spacing; // Don't count trailing spacing
            maxLineLength = util::max(lineLength, maxLineLength);
            
            justifyLine(shaping.positionedGlyphs, glyphs, lineStartIndex,
                        shaping.positionedGlyphs.size() - 1, justify);
        }
        
        x = 0;
        y += lineHeight;
    }

    auto anchorAlign = getAnchorAlignment(textAnchor);

    align(shaping, justify, anchorAlign.horizontalAlign, anchorAlign.verticalAlign, maxLineLength,
          lineHeight, lines.size());
    const float height = lines.size() * lineHeight;

    // Calculate the bounding box
    shaping.top += -anchorAlign.verticalAlign * height;
    shaping.bottom = shaping.top + height;
    shaping.left += -anchorAlign.horizontalAlign * maxLineLength;
    shaping.right = shaping.left + maxLineLength;
}

const Shaping getShaping(const std::u16string& logicalInput,
                         const float maxWidth,
                         const float lineHeight,
                         const style::SymbolAnchorType textAnchor,
                         const style::TextJustifyType textJustify,
                         const float spacing,
                         const Point<float>& translate,
                         const float verticalHeight,
                         const WritingModeType writingMode,
                         BiDi& bidi,
                         const Glyphs& glyphs) {
    Shaping shaping(translate.x, translate.y, writingMode);
    
    std::vector<std::u16string> reorderedLines =
    bidi.processText(logicalInput,
                     determineLineBreaks(logicalInput, spacing, maxWidth, writingMode, glyphs));
    
    shapeLines(shaping, reorderedLines, spacing, lineHeight, textAnchor,
               textJustify, verticalHeight, writingMode, glyphs);
    
    return shaping;
}


} // namespace mbgl
