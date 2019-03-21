#include <mbgl/text/shaping.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/layout/symbol_feature.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/text/bidi.hpp>

#include <algorithm>
#include <list>
#include <cmath>

namespace mbgl {


// static
AnchorAlignment AnchorAlignment::getAnchorAlignment(style::SymbolAnchorType anchor) {
    AnchorAlignment result(0.5f, 0.5f);

    switch (anchor) {
    case style::SymbolAnchorType::Right:
    case style::SymbolAnchorType::TopRight:
    case style::SymbolAnchorType::BottomRight:
        result.horizontalAlign = 1.0f;
        break;
    case style::SymbolAnchorType::Left:
    case style::SymbolAnchorType::TopLeft:
    case style::SymbolAnchorType::BottomLeft:
        result.horizontalAlign = 0.0f;
        break;
    default:
	break;
    }

    switch (anchor) {
    case style::SymbolAnchorType::Bottom:
    case style::SymbolAnchorType::BottomLeft:
    case style::SymbolAnchorType::BottomRight:
        result.verticalAlign = 1.0f;
        break;
    case style::SymbolAnchorType::Top:
    case style::SymbolAnchorType::TopLeft:
    case style::SymbolAnchorType::TopRight:
        result.verticalAlign = 0.0f;
        break;
    default:
        break;
    }

    return result;
}

style::TextJustifyType getAnchorJustification(style::SymbolAnchorType anchor) {
    switch (anchor) {
    case style::SymbolAnchorType::Right:
    case style::SymbolAnchorType::TopRight:
    case style::SymbolAnchorType::BottomRight:
        return style::TextJustifyType::Right;
    case style::SymbolAnchorType::Left:
    case style::SymbolAnchorType::TopLeft:
    case style::SymbolAnchorType::BottomLeft:
        return style::TextJustifyType::Left;
    default:
        return style::TextJustifyType::Center;
    }
}

PositionedIcon PositionedIcon::shapeIcon(const ImagePosition& image, const std::array<float, 2>& iconOffset, style::SymbolAnchorType iconAnchor, const float iconRotation) {
    AnchorAlignment anchorAlign = AnchorAlignment::getAnchorAlignment(iconAnchor);
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
                 const GlyphMap& glyphMap,
                 std::size_t start,
                 std::size_t end,
                 float justify) {
    if (!justify) {
        return;
    }
    
    PositionedGlyph& glyph = positionedGlyphs[end];
    auto glyphs = glyphMap.find(glyph.font);
    if (glyphs == glyphMap.end()) {
        return;
    }
    auto it = glyphs->second.find(glyph.glyph);
    if (it != glyphs->second.end() && it->second) {
        const float lastAdvance = (*it->second)->metrics.advance * glyph.scale;
        const float lineIndent = float(glyph.x + lastAdvance) * justify;
        
        for (std::size_t j = start; j <= end; j++) {
            positionedGlyphs[j].x -= lineIndent;
        }
    }
}

float determineAverageLineWidth(const TaggedString& logicalInput,
                                const float spacing,
                                float maxWidth,
                                const GlyphMap& glyphMap) {
    float totalWidth = 0;
    
    for (std::size_t i = 0; i < logicalInput.length(); i++) {
        const SectionOptions& section = logicalInput.getSection(i);
        char16_t codePoint = logicalInput.getCharCodeAt(i);
        auto glyphs = glyphMap.find(section.fontStackHash);
        if (glyphs == glyphMap.end()) {
            continue;
        }
        auto it = glyphs->second.find(codePoint);
        if (it == glyphs->second.end() || !it->second) {
            continue;
        }
        
        totalWidth += (*it->second)->metrics.advance * section.scale + spacing;
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
std::set<std::size_t> determineLineBreaks(const TaggedString& logicalInput,
                                          const float spacing,
                                          float maxWidth,
                                          const WritingModeType writingMode,
                                          const GlyphMap& glyphMap) {
    if (!maxWidth || writingMode != WritingModeType::Horizontal) {
        return {};
    }
    
    if (logicalInput.empty()) {
        return {};
    }
    
    const float targetWidth = determineAverageLineWidth(logicalInput, spacing, maxWidth, glyphMap);
    
    std::list<PotentialBreak> potentialBreaks;
    float currentX = 0;
    
    for (std::size_t i = 0; i < logicalInput.length(); i++) {
        const SectionOptions& section = logicalInput.getSection(i);
        char16_t codePoint = logicalInput.getCharCodeAt(i);
        auto glyphs = glyphMap.find(section.fontStackHash);
        if (glyphs == glyphMap.end()) {
            continue;
        }
        auto it = glyphs->second.find(codePoint);
        if (it != glyphs->second.end() && it->second && !util::i18n::isWhitespace(codePoint)) {
            currentX += (*it->second)->metrics.advance * section.scale + spacing;
        }
        
        // Ideographic characters, spaces, and word-breaking punctuation that often appear without
        // surrounding spaces.
        if ((i < logicalInput.length() - 1) &&
            (util::i18n::allowsWordBreaking(codePoint) || util::i18n::allowsIdeographicBreaking(codePoint))) {
            potentialBreaks.push_back(evaluateBreak(i+1, currentX, targetWidth, potentialBreaks,
                                                    calculatePenalty(codePoint, logicalInput.getCharCodeAt(i+1)),
                                                    false));
        }
    }
    
    return leastBadBreaks(evaluateBreak(logicalInput.length(), currentX, targetWidth, potentialBreaks, 0, true));
}

void shapeLines(Shaping& shaping,
                std::vector<TaggedString>& lines,
                const float spacing,
                const float lineHeight,
                const style::SymbolAnchorType textAnchor,
                const style::TextJustifyType textJustify,
                const WritingModeType writingMode,
                const GlyphMap& glyphMap) {
    
    // the y offset *should* be part of the font metadata
    const int32_t yOffset = -17;
    
    float x = 0;
    float y = yOffset;
    
    float maxLineLength = 0;


    const float justify = textJustify == style::TextJustifyType::Right ? 1 :
        textJustify == style::TextJustifyType::Left ? 0 :
        0.5;
    
    for (TaggedString& line : lines) {
        // Collapse whitespace so it doesn't throw off justification
        line.trim();
        
        const double lineMaxScale = line.getMaxScale();
        
        if (line.empty()) {
            y += lineHeight; // Still need a line feed after empty line
            continue;
        }
        
        std::size_t lineStartIndex = shaping.positionedGlyphs.size();
        for (std::size_t i = 0; i < line.length(); i++) {
            const std::size_t sectionIndex = line.getSectionIndex(i);
            const SectionOptions& section = line.sectionAt(sectionIndex);
            char16_t codePoint = line.getCharCodeAt(i);
            auto glyphs = glyphMap.find(section.fontStackHash);
            if (glyphs == glyphMap.end()) {
                continue;
            }
            auto it = glyphs->second.find(codePoint);
            if (it == glyphs->second.end() || !it->second) {
                continue;
            }
            
            // We don't know the baseline, but since we're laying out
            // at 24 points, we can calculate how much it will move when
            // we scale up or down.
            const double baselineOffset = (lineMaxScale - section.scale) * util::ONE_EM;
            
            const Glyph& glyph = **it->second;
            
            if (writingMode == WritingModeType::Horizontal || !util::i18n::hasUprightVerticalOrientation(codePoint)) {
                shaping.positionedGlyphs.emplace_back(codePoint, x, y + baselineOffset, false, section.fontStackHash, section.scale, sectionIndex);
                x += glyph.metrics.advance * section.scale + spacing;
            } else {
                shaping.positionedGlyphs.emplace_back(codePoint, x, baselineOffset, true, section.fontStackHash, section.scale, sectionIndex);
                x += util::ONE_EM * section.scale + spacing;
            }
        }
        
        // Only justify if we placed at least one glyph
        if (shaping.positionedGlyphs.size() != lineStartIndex) {
            float lineLength = x - spacing; // Don't count trailing spacing
            maxLineLength = util::max(lineLength, maxLineLength);
            
            justifyLine(shaping.positionedGlyphs, glyphMap, lineStartIndex,
                        shaping.positionedGlyphs.size() - 1, justify);
        }
        
        x = 0;
        y += lineHeight * lineMaxScale;
    }

    auto anchorAlign = AnchorAlignment::getAnchorAlignment(textAnchor);

    align(shaping, justify, anchorAlign.horizontalAlign, anchorAlign.verticalAlign, maxLineLength,
          lineHeight, lines.size());
    const float height = y - yOffset;

    // Calculate the bounding box
    shaping.top += -anchorAlign.verticalAlign * height;
    shaping.bottom = shaping.top + height;
    shaping.left += -anchorAlign.horizontalAlign * maxLineLength;
    shaping.right = shaping.left + maxLineLength;
}

const Shaping getShaping(const TaggedString& formattedString,
                         const float maxWidth,
                         const float lineHeight,
                         const style::SymbolAnchorType textAnchor,
                         const style::TextJustifyType textJustify,
                         const float spacing,
                         const Point<float>& translate,
                         //const float verticalHeight,
                         const WritingModeType writingMode,
                         BiDi& bidi,
                         const GlyphMap& glyphs) {   
    std::vector<TaggedString> reorderedLines;
    if (formattedString.sectionCount() == 1) {
        auto untaggedLines = bidi.processText(formattedString.rawText(),
                                              determineLineBreaks(formattedString, spacing, maxWidth, writingMode, glyphs));
        for (const auto& line : untaggedLines) {
            reorderedLines.emplace_back(line, formattedString.sectionAt(0));
        }
    } else {
        auto processedLines = bidi.processStyledText(formattedString.getStyledText(),
                                                     determineLineBreaks(formattedString, spacing, maxWidth, writingMode, glyphs));
        for (const auto& line : processedLines) {
            reorderedLines.emplace_back(line, formattedString.getSections());
        }
    }
    Shaping shaping(translate.x, translate.y, writingMode, reorderedLines.size());
    shapeLines(shaping, reorderedLines, spacing, lineHeight, textAnchor,
               textJustify, writingMode, glyphs);
    
    return shaping;
}


} // namespace mbgl
