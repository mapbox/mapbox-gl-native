#include <mbgl/text/quads.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/geometry/anchor.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/optional.hpp>

#include <cassert>

namespace mbgl {

using namespace style;

SymbolQuad getIconQuad(const PositionedIcon& shapedIcon,
                       const SymbolLayoutProperties::Evaluated& layout,
                       const float layoutTextSize,
                       const Shaping& shapedText) {
    const ImagePosition& image = shapedIcon.image();

    // If you have a 10px icon that isn't perfectly aligned to the pixel grid it will cover 11 actual
    // pixels. The quad needs to be padded to account for this, otherwise they'll look slightly clipped
    // on one edge in some cases.
    const float border = 1.0;

    float top = shapedIcon.top() - border / image.pixelRatio;
    float left = shapedIcon.left() - border / image.pixelRatio;
    float bottom = shapedIcon.bottom() + border / image.pixelRatio;
    float right = shapedIcon.right() + border / image.pixelRatio;
    Point<float> tl;
    Point<float> tr;
    Point<float> br;
    Point<float> bl;

    if (layout.get<IconTextFit>() != IconTextFitType::None && shapedText) {
        auto iconWidth = right - left;
        auto iconHeight = bottom - top;
        auto size = layoutTextSize / 24.0f;
        auto textLeft = shapedText.left * size;
        auto textRight = shapedText.right * size;
        auto textTop = shapedText.top * size;
        auto textBottom = shapedText.bottom * size;
        auto textWidth = textRight - textLeft;
        auto textHeight = textBottom - textTop;
        auto padT = layout.get<IconTextFitPadding>()[0];
        auto padR = layout.get<IconTextFitPadding>()[1];
        auto padB = layout.get<IconTextFitPadding>()[2];
        auto padL = layout.get<IconTextFitPadding>()[3];
        auto offsetY = layout.get<IconTextFit>() == IconTextFitType::Width ? (textHeight - iconHeight) * 0.5 : 0;
        auto offsetX = layout.get<IconTextFit>() == IconTextFitType::Height ? (textWidth - iconWidth) * 0.5 : 0;
        auto width = layout.get<IconTextFit>() == IconTextFitType::Width || layout.get<IconTextFit>() == IconTextFitType::Both ? textWidth : iconWidth;
        auto height = layout.get<IconTextFit>() == IconTextFitType::Height || layout.get<IconTextFit>() == IconTextFitType::Both ? textHeight : iconHeight;
        left = textLeft + offsetX - padL;
        top = textTop + offsetY - padT;
        right = textLeft + offsetX + padR + width;
        bottom = textTop + offsetY + padB + height;
        tl = {left, top};
        tr = {right, top};
        br = {right, bottom};
        bl = {left, bottom};
    } else {
        tl = {left, top};
        tr = {right, top};
        br = {right, bottom};
        bl = {left, bottom};
    }

    const float angle = shapedIcon.angle();

    if (angle) {
        // Compute the transformation matrix.
        float angle_sin = std::sin(angle);
        float angle_cos = std::cos(angle);
        std::array<float, 4> matrix = {{angle_cos, -angle_sin, angle_sin, angle_cos}};

        tl = util::matrixMultiply(matrix, tl);
        tr = util::matrixMultiply(matrix, tr);
        bl = util::matrixMultiply(matrix, bl);
        br = util::matrixMultiply(matrix, br);
    }

    // Icon quad is padded, so texture coordinates also need to be padded.
    Rect<uint16_t> textureRect {
        static_cast<uint16_t>(image.textureRect.x - border),
        static_cast<uint16_t>(image.textureRect.y - border),
        static_cast<uint16_t>(image.textureRect.w + border * 2),
        static_cast<uint16_t>(image.textureRect.h + border * 2)
    };

    return SymbolQuad { tl, tr, bl, br, textureRect, shapedText.writingMode, { 0.0f, 0.0f } };
}

SymbolQuads getGlyphQuads(const Shaping& shapedText,
                          const SymbolLayoutProperties::Evaluated& layout,
                          const style::SymbolPlacementType placement,
                          const GlyphPositionMap& positions) {
    const float textRotate = layout.get<TextRotate>() * util::DEG2RAD;

    const float oneEm = 24.0;
    std::array<float, 2> textOffset = layout.get<TextOffset>();
    textOffset[0] *= oneEm;
    textOffset[1] *= oneEm;

    SymbolQuads quads;

    for (const PositionedGlyph &positionedGlyph: shapedText.positionedGlyphs) {
        auto positionsIt = positions.find(positionedGlyph.glyph);
        if (positionsIt == positions.end())
            continue;

        const GlyphPosition& glyph = positionsIt->second;
        const Rect<uint16_t>& rect = glyph.rect;

        // The rects have an addditional buffer that is not included in their size;
        const float glyphPadding = 1.0f;
        const float rectBuffer = 3.0f + glyphPadding;

        const float halfAdvance = glyph.metrics.advance / 2.0;
        const bool alongLine = layout.get<TextRotationAlignment>() == AlignmentType::Map && placement == SymbolPlacementType::Line;

        const Point<float> glyphOffset = alongLine ?
            Point<float>{ positionedGlyph.x + halfAdvance, positionedGlyph.y } :
            Point<float>{ 0.0f, 0.0f };

        const Point<float> builtInOffset = alongLine ?
            Point<float>{ 0.0f, 0.0f } :
            Point<float>{ positionedGlyph.x + halfAdvance + textOffset[0], positionedGlyph.y + textOffset[1] };


        const float x1 = glyph.metrics.left - rectBuffer - halfAdvance + builtInOffset.x;
        const float y1 = -glyph.metrics.top - rectBuffer + builtInOffset.y;
        const float x2 = x1 + rect.w;
        const float y2 = y1 + rect.h;

        const Point<float> center{builtInOffset.x - halfAdvance, static_cast<float>(static_cast<float>(glyph.metrics.advance) / 2.0)};

        Point<float> tl{x1, y1};
        Point<float> tr{x2, y1};
        Point<float> bl{x1, y2};
        Point<float> br{x2, y2};

        if (positionedGlyph.angle != 0) {
            tl = util::rotate(tl - center, positionedGlyph.angle) + center;
            tr = util::rotate(tr - center, positionedGlyph.angle) + center;
            bl = util::rotate(bl - center, positionedGlyph.angle) + center;
            br = util::rotate(br - center, positionedGlyph.angle) + center;
        }

        if (textRotate) {
            // Compute the transformation matrix.
            float angle_sin = std::sin(textRotate);
            float angle_cos = std::cos(textRotate);
            std::array<float, 4> matrix = {{angle_cos, -angle_sin, angle_sin, angle_cos}};

            tl = util::matrixMultiply(matrix, tl);
            tr = util::matrixMultiply(matrix, tr);
            bl = util::matrixMultiply(matrix, bl);
            br = util::matrixMultiply(matrix, br);
        }

        quads.emplace_back(tl, tr, bl, br, rect, shapedText.writingMode, glyphOffset);
    }

    return quads;
}
} // namespace mbgl
