#include <mbgl/text/font_stack.hpp>

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
    Shaping shaping;

    int32_t x = std::round(translate.x * 24); // one em
    const int32_t y = std::round(translate.y * 24); // one em

    // Loop through all characters of this label and shape.
    for (uint32_t chr : string) {
        shaping.emplace_back(chr, x, y);
        auto metric = metrics.find(chr);
        if (metric != metrics.end()) {
            x += metric->second.advance + spacing;
        }
    }

    if (!shaping.size())
        return shaping;

    lineWrap(shaping, lineHeight, maxWidth, horizontalAlign, verticalAlign, justify);

    return shaping;
}

void align(Shaping &shaping, const float justify, const float horizontalAlign,
           const float verticalAlign, const uint32_t maxLineLength, const float lineHeight,
           const uint32_t line) {
    const float shiftX = (justify - horizontalAlign) * maxLineLength;
    const float shiftY = (-verticalAlign * (line + 1) + 0.5) * lineHeight;

    for (auto& glyph : shaping) {
        glyph.x += shiftX;
        glyph.y += shiftY;
    }
}

void justifyLine(Shaping &shaping, const std::map<uint32_t, GlyphMetrics> &metrics, uint32_t start,
                 uint32_t end, float justify) {
    PositionedGlyph &glyph = shaping[end];
    auto metric = metrics.find(glyph.glyph);
    if (metric != metrics.end()) {
        const uint32_t lastAdvance = metric->second.advance;
        const float lineIndent = float(glyph.x + lastAdvance) * justify;

        for (uint32_t j = start; j <= end; j++) {
            shaping[j].x -= lineIndent;
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

    if (maxWidth) {
        for (uint32_t i = 0; i < shaping.size(); i++) {
            PositionedGlyph &shape = shaping[i];

            shape.x -= lengthBeforeCurrentLine;
            shape.y += lineHeight * line;

            if (shape.x > maxWidth && lastSafeBreak > 0) {

                uint32_t lineLength = shaping[lastSafeBreak + 1].x;
                maxLineLength = util::max(lineLength, maxLineLength);

                for (uint32_t k = lastSafeBreak + 1; k <= i; k++) {
                    shaping[k].y += lineHeight;
                    shaping[k].x -= lineLength;
                }

                if (justify) {
                    justifyLine(shaping, metrics, lineStartIndex, lastSafeBreak - 1, justify);
                }

                lineStartIndex = lastSafeBreak + 1;
                lastSafeBreak = 0;
                lengthBeforeCurrentLine += lineLength;
                line++;
            }

            if (shape.glyph == 32) {
                lastSafeBreak = i;
            }
        }
    }

    if (!maxLineLength) maxLineLength = shaping.back().x;

    justifyLine(shaping, metrics, lineStartIndex, uint32_t(shaping.size()) - 1, justify);
    align(shaping, justify, horizontalAlign, verticalAlign, maxLineLength, lineHeight, line);
}

} // end namespace mbgl
