#include <mbgl/text/font_stack.hpp>
#include <cassert>
#include <mbgl/util/math.hpp>

namespace mbgl {

void FontStack::insert(uint32_t id, const SDFGlyph &glyph) {
    metrics.emplace(id, glyph.metrics);
    bitmaps.emplace(id, glyph.bitmap);
    sdfs.emplace(id, glyph);
}

const std::map<uint32_t, GlyphMetrics> &FontStack::getMetrics() const {
    return metrics;
}

const std::map<uint32_t, SDFGlyph> &FontStack::getSDFs() const {
    return sdfs;
}

const Shaping FontStack::getShaping(const std::u32string &string, const float maxWidth,
                                    const float lineHeight, const float horizontalAlign,
                                    const float verticalAlign, const float justify,
                                    const float spacing, const vec2<float> &translate) const {
    Shaping shaping(translate.x * 24, translate.y * 24, string);

    // the y offset *should* be part of the font metadata
    const int32_t yOffset = -17;

    float x = ::round(translate.x * 24); // one em
    const float y = ::round(translate.y * 24) + yOffset; // one em

    // Loop through all characters of this label and shape.
    for (uint32_t chr : string) {
        auto metric = metrics.find(chr);
        if (metric != metrics.end()) {
            shaping.positionedGlyphs.emplace_back(chr, x, y);
            x += metric->second.advance + spacing;
        }
    }

    if (shaping.positionedGlyphs.empty())
        return shaping;

    lineWrap(shaping, lineHeight, maxWidth, horizontalAlign, verticalAlign, justify);

    return shaping;
}

void align(Shaping &shaping, const float justify, const float horizontalAlign,
           const float verticalAlign, const uint32_t maxLineLength, const float lineHeight,
           const uint32_t line) {
    const float shiftX = (justify - horizontalAlign) * maxLineLength;
    const float shiftY = (-verticalAlign * (line + 1) + 0.5) * lineHeight;

    for (auto& glyph : shaping.positionedGlyphs) {
        glyph.x += shiftX;
        glyph.y += shiftY;
    }
}

void justifyLine(std::vector<PositionedGlyph> &positionedGlyphs, const std::map<uint32_t, GlyphMetrics> &metrics, uint32_t start,
                 uint32_t end, float justify) {
    PositionedGlyph &glyph = positionedGlyphs[end];
    auto metric = metrics.find(glyph.glyph);
    if (metric != metrics.end()) {
        const uint32_t lastAdvance = metric->second.advance;
        const float lineIndent = float(glyph.x + lastAdvance) * justify;

        for (uint32_t j = start; j <= end; j++) {
            positionedGlyphs[j].x -= lineIndent;
        }
    }
}

void FontStack::lineWrap(Shaping &shaping, const float lineHeight, const float maxWidth,
                         const float horizontalAlign, const float verticalAlign,
                         const float justify) const {
    uint32_t lastSafeBreak = 0;

    uint32_t lengthBeforeCurrentLine = 0;
    uint32_t lineStartIndex = 0;
    uint32_t line = 0;

    uint32_t maxLineLength = 0;

    std::vector<PositionedGlyph> &positionedGlyphs = shaping.positionedGlyphs;

    if (maxWidth) {
        for (uint32_t i = 0; i < positionedGlyphs.size(); i++) {
            PositionedGlyph &shape = positionedGlyphs[i];

            shape.x -= lengthBeforeCurrentLine;
            shape.y += lineHeight * line;

            if (shape.x > maxWidth && lastSafeBreak > 0) {

                uint32_t lineLength = positionedGlyphs[lastSafeBreak + 1].x;
                maxLineLength = util::max(lineLength, maxLineLength);

                for (uint32_t k = lastSafeBreak + 1; k <= i; k++) {
                    positionedGlyphs[k].y += lineHeight;
                    positionedGlyphs[k].x -= lineLength;
                }

                if (justify) {
                    // Collapse invisible characters.
                    uint32_t breakGlyph = positionedGlyphs[lastSafeBreak].glyph;
                    uint32_t lineEnd = lastSafeBreak;
                    if (breakGlyph == 0x20 /* space */
                        || breakGlyph == 0x200b /* zero-width space */) {
                        lineEnd--;
                    }

                    justifyLine(positionedGlyphs, metrics, lineStartIndex, lineEnd, justify);
                }

                lineStartIndex = lastSafeBreak + 1;
                lastSafeBreak = 0;
                lengthBeforeCurrentLine += lineLength;
                line++;
            }

            // Spaces, plus word-breaking punctuation that often appears without surrounding spaces.
            if (shape.glyph == 0x20 /* space */
                || shape.glyph == 0x26 /* ampersand */
                || shape.glyph == 0x2b /* plus sign */
                || shape.glyph == 0x2d /* hyphen-minus */
                || shape.glyph == 0x2f /* solidus */
                || shape.glyph == 0xad /* soft hyphen */
                || shape.glyph == 0xb7 /* middle dot */
                || shape.glyph == 0x200b /* zero-width space */
                || shape.glyph == 0x2010 /* hyphen */
                || shape.glyph == 0x2013 /* en dash */) {
                lastSafeBreak = i;
            }
        }
    }

    const PositionedGlyph& lastPositionedGlyph = positionedGlyphs.back();
    const auto lastGlyphMetric = metrics.find(lastPositionedGlyph.glyph);
    assert(lastGlyphMetric != metrics.end());
    const uint32_t lastLineLength = lastPositionedGlyph.x + lastGlyphMetric->second.advance;
    maxLineLength = std::max(maxLineLength, lastLineLength);

    const uint32_t height = (line + 1) * lineHeight;

    justifyLine(positionedGlyphs, metrics, lineStartIndex, uint32_t(positionedGlyphs.size()) - 1, justify);
    align(shaping, justify, horizontalAlign, verticalAlign, maxLineLength, lineHeight, line);

    // Calculate the bounding box
    shaping.top += -verticalAlign * height;
    shaping.bottom = shaping.top + height;
    shaping.left += -horizontalAlign * maxLineLength;
    shaping.right = shaping.left + maxLineLength;
}

} // end namespace mbgl
