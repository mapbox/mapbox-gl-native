#include <mbgl/text/glyph_set.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/math/minmax.hpp>

#include <cassert>

namespace mbgl {

void GlyphSet::insert(uint32_t id, SDFGlyph&& glyph) {
    auto it = sdfs.find(id);
    if (it == sdfs.end()) {
        // Glyph doesn't exist yet.
        sdfs.emplace(id, std::move(glyph));
    } else if (it->second.metrics == glyph.metrics) {
        if (it->second.bitmap != glyph.bitmap) {
            // The actual bitmap was updated; this is unsupported.
            Log::Warning(Event::Glyph, "Modified glyph changed bitmap represenation");
        }
        // At least try to update it in case it's currently unsused.
        // If it is already used; we won't attempt to update the glyph atlas texture.
        it->second.bitmap = std::move(glyph.bitmap);
    } else {
        // The metrics were updated; this is unsupported.
        Log::Warning(Event::Glyph, "Modified glyph has different metrics");
        return;
    }
}

const std::map<uint32_t, SDFGlyph>& GlyphSet::getSDFs() const {
    return sdfs;
}

const Shaping GlyphSet::getShaping(const std::u32string& string,
                                   const float maxWidth,
                                   const float lineHeight,
                                   const float horizontalAlign,
                                   const float verticalAlign,
                                   const float justify,
                                   const float spacing,
                                   const Point<float>& translate) const {
    Shaping shaping(translate.x * 24, translate.y * 24, string);

    // the y offset *should* be part of the font metadata
    const int32_t yOffset = -17;

    float x = 0;
    const float y = yOffset;

    // Loop through all characters of this label and shape.
    for (uint32_t chr : string) {
        auto it = sdfs.find(chr);
        if (it != sdfs.end()) {
            shaping.positionedGlyphs.emplace_back(chr, x, y);
            x += it->second.metrics.advance + spacing;
        }
    }

    if (shaping.positionedGlyphs.empty()) {
        return shaping;
    }

    lineWrap(shaping, lineHeight, maxWidth, horizontalAlign, verticalAlign, justify, translate);

    return shaping;
}

void align(Shaping& shaping,
           const float justify,
           const float horizontalAlign,
           const float verticalAlign,
           const uint32_t maxLineLength,
           const float lineHeight,
           const uint32_t line,
           const Point<float>& translate) {
    const float shiftX =
        (justify - horizontalAlign) * maxLineLength + ::round(translate.x * 24 /* one em */);
    const float shiftY =
        (-verticalAlign * (line + 1) + 0.5) * lineHeight + ::round(translate.y * 24 /* one em */);

    for (auto& glyph : shaping.positionedGlyphs) {
        glyph.x += shiftX;
        glyph.y += shiftY;
    }
}

void justifyLine(std::vector<PositionedGlyph>& positionedGlyphs,
                 const std::map<uint32_t, SDFGlyph>& sdfs,
                 uint32_t start,
                 uint32_t end,
                 float justify) {
    PositionedGlyph& glyph = positionedGlyphs[end];
    auto it = sdfs.find(glyph.glyph);
    if (it != sdfs.end()) {
        const uint32_t lastAdvance = it->second.metrics.advance;
        const float lineIndent = float(glyph.x + lastAdvance) * justify;

        for (uint32_t j = start; j <= end; j++) {
            positionedGlyphs[j].x -= lineIndent;
        }
    }
}

void GlyphSet::lineWrap(Shaping& shaping,
                        const float lineHeight,
                        const float maxWidth,
                        const float horizontalAlign,
                        const float verticalAlign,
                        const float justify,
                        const Point<float>& translate) const {
    uint32_t lastSafeBreak = 0;

    uint32_t lengthBeforeCurrentLine = 0;
    uint32_t lineStartIndex = 0;
    uint32_t line = 0;

    uint32_t maxLineLength = 0;

    std::vector<PositionedGlyph>& positionedGlyphs = shaping.positionedGlyphs;

    if (maxWidth) {
        for (uint32_t i = 0; i < positionedGlyphs.size(); i++) {
            PositionedGlyph& shape = positionedGlyphs[i];

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

                    justifyLine(positionedGlyphs, sdfs, lineStartIndex, lineEnd, justify);
                }

                lineStartIndex = lastSafeBreak + 1;
                lastSafeBreak = 0;
                lengthBeforeCurrentLine += lineLength;
                line++;
            }

            // Spaces, plus word-breaking punctuation that often appears without surrounding spaces.
            if (shape.glyph == 0x20      /* space */
                || shape.glyph == 0x26   /* ampersand */
                || shape.glyph == 0x2b   /* plus sign */
                || shape.glyph == 0x2d   /* hyphen-minus */
                || shape.glyph == 0x2f   /* solidus */
                || shape.glyph == 0xad   /* soft hyphen */
                || shape.glyph == 0xb7   /* middle dot */
                || shape.glyph == 0x200b /* zero-width space */
                || shape.glyph == 0x2010 /* hyphen */
                || shape.glyph == 0x2013 /* en dash */) {
                lastSafeBreak = i;
            }
        }
    }

    const PositionedGlyph& lastPositionedGlyph = positionedGlyphs.back();
    const auto lastGlyphIt = sdfs.find(lastPositionedGlyph.glyph);
    assert(lastGlyphIt != sdfs.end());
    const uint32_t lastLineLength = lastPositionedGlyph.x + lastGlyphIt->second.metrics.advance;
    maxLineLength = std::max(maxLineLength, lastLineLength);

    const uint32_t height = (line + 1) * lineHeight;

    justifyLine(positionedGlyphs, sdfs, lineStartIndex, uint32_t(positionedGlyphs.size()) - 1,
                justify);
    align(shaping, justify, horizontalAlign, verticalAlign, maxLineLength, lineHeight, line,
          translate);

    // Calculate the bounding box
    shaping.top += -verticalAlign * height;
    shaping.bottom = shaping.top + height;
    shaping.left += -horizontalAlign * maxLineLength;
    shaping.right = shaping.left + maxLineLength;
}

} // end namespace mbgl
