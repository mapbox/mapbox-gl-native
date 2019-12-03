#include <mbgl/geometry/anchor.hpp>
#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/text/quads.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/optional.hpp>

#include <cassert>

namespace mbgl {

using namespace style;

SymbolQuad getIconQuad(const PositionedIcon& shapedIcon, WritingModeType writingMode, SymbolContent iconType) {
    const ImagePosition& image = shapedIcon.image();

    // If you have a 10px icon that isn't perfectly aligned to the pixel grid it will cover 11 actual
    // pixels. The quad needs to be padded to account for this, otherwise they'll look slightly clipped
    // on one edge in some cases.
    const float border = ImagePosition::padding;

    // Expand the box to respect the 1 pixel border in the atlas image. We're using `image.paddedRect - border`
    // instead of image.displaySize because we only pad with one pixel for retina images as well, and the
    // displaySize uses the logical dimensions, not the physical pixel dimensions.
    // Unlike the JavaScript version, we're _not_ including the padding in the texture rect, so the
    // logic "dimension * padded / non-padded - dimension" is swapped.
    const float iconWidth = shapedIcon.right() - shapedIcon.left();
    const float expandX = (iconWidth * (static_cast<float>(image.textureRect.w) + 2.0f * border) /
                               static_cast<float>(image.textureRect.w) -
                           iconWidth) /
                          2.0f;
    const float left = shapedIcon.left() - expandX;
    const float right = shapedIcon.right() + expandX;

    const float iconHeight = shapedIcon.bottom() - shapedIcon.top();
    const float expandY = (iconHeight * (static_cast<float>(image.textureRect.h) + 2.0f * border) /
                               static_cast<float>(image.textureRect.h) -
                           iconHeight) /
                          2.0f;
    const float top = shapedIcon.top() - expandY;
    const float bottom = shapedIcon.bottom() + expandY;

    Point<float> tl{left, top};
    Point<float> tr{right, top};
    Point<float> br{right, bottom};
    Point<float> bl{left, bottom};

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

    return SymbolQuad{tl, tr, bl, br, textureRect, writingMode, {0.0f, 0.0f}, iconType == SymbolContent::IconSDF};
}

SymbolQuads getGlyphQuads(const Shaping& shapedText,
                          const std::array<float, 2> textOffset,
                          const SymbolLayoutProperties::Evaluated& layout,
                          const style::SymbolPlacementType placement,
                          const ImageMap& imageMap,
                          bool allowVerticalPlacement) {
    const float textRotate = layout.get<TextRotate>() * util::DEG2RAD;
    const bool alongLine = layout.get<TextRotationAlignment>() == AlignmentType::Map && placement != SymbolPlacementType::Point;

    SymbolQuads quads;

    for (const auto& line : shapedText.positionedLines) {
        for (const auto& positionedGlyph : line.positionedGlyphs) {
            if (!positionedGlyph.rect.hasArea()) continue;

            // The rects have an addditional buffer that is not included in their size;
            const float glyphPadding = 1.0f;
            float rectBuffer = 3.0f + glyphPadding;
            float pixelRatio = 1.0f;
            float lineOffset = 0.0f;
            const bool rotateVerticalGlyph = (alongLine || allowVerticalPlacement) && positionedGlyph.vertical;
            const float halfAdvance = positionedGlyph.metrics.advance * positionedGlyph.scale / 2.0;
            const Rect<uint16_t>& rect = positionedGlyph.rect;
            bool isSDF = true;

            // Align images and scaled glyphs in the middle of a vertical line.
            if (allowVerticalPlacement && shapedText.verticalizable) {
                const float scaledGlyphOffset = (positionedGlyph.scale - 1) * util::ONE_EM;
                const float imageOffset = (util::ONE_EM - positionedGlyph.metrics.width * positionedGlyph.scale) / 2.0f;
                lineOffset = line.lineOffset / 2.0f - (positionedGlyph.imageID ? -imageOffset : scaledGlyphOffset);
            }

            if (positionedGlyph.imageID) {
                auto image = imageMap.find(*positionedGlyph.imageID);
                if (image == imageMap.end()) {
                    continue;
                }
                pixelRatio = image->second->pixelRatio;
                rectBuffer = ImagePosition::padding / pixelRatio;
                isSDF = image->second->sdf;
            }

            const Point<float> glyphOffset =
                alongLine ? Point<float>{positionedGlyph.x + halfAdvance, positionedGlyph.y} : Point<float>{0.0f, 0.0f};

            Point<float> builtInOffset = alongLine ? Point<float>{0.0f, 0.0f}
                                                   : Point<float>{positionedGlyph.x + halfAdvance + textOffset[0],
                                                                  positionedGlyph.y + textOffset[1] - lineOffset};

            Point<float> verticalizedLabelOffset = {0.0f, 0.0f};
            if (rotateVerticalGlyph) {
                // Vertical POI labels, that are rotated 90deg CW and whose glyphs must preserve upright orientation
                // need to be rotated 90deg CCW. After quad is rotated, it is translated to the original built-in
                // offset.
                verticalizedLabelOffset = builtInOffset;
                builtInOffset = {0.0f, 0.0f};
            }

            const float x1 =
                (positionedGlyph.metrics.left - rectBuffer) * positionedGlyph.scale - halfAdvance + builtInOffset.x;
            const float y1 = (-positionedGlyph.metrics.top - rectBuffer) * positionedGlyph.scale + builtInOffset.y;
            const float x2 = x1 + rect.w * positionedGlyph.scale / pixelRatio;
            const float y2 = y1 + rect.h * positionedGlyph.scale / pixelRatio;

            Point<float> tl{x1, y1};
            Point<float> tr{x2, y1};
            Point<float> bl{x1, y2};
            Point<float> br{x2, y2};

            if (rotateVerticalGlyph) {
                // Vertical-supporting glyphs are laid out in 24x24 point boxes (1 square em)
                // In horizontal orientation, the y values for glyphs are below the midline
                // and we use a "yOffset" of -17 to pull them up to the middle.
                // By rotating counter-clockwise around the point at the center of the left
                // edge of a 24x24 layout box centered below the midline, we align the center
                // of the glyphs with the horizontal midline, so the yOffset is no longer
                // necessary, but we also pull the glyph to the left along the x axis.
                // The y coordinate includes baseline yOffset, therefore, needs to be accounted
                // for when glyph is rotated and translated.

                const Point<float> center{-halfAdvance, halfAdvance - Shaping::yOffset};
                const float verticalRotation = -M_PI_2;

                // xHalfWidhtOffsetcorrection is a difference between full-width and half-width
                // advance, should be 0 for full-width glyphs and will pull up half-width glyphs.
                const float xHalfWidhtOffsetcorrection = util::ONE_EM / 2 - halfAdvance;
                const float yImageOffsetCorrection = positionedGlyph.imageID ? xHalfWidhtOffsetcorrection : 0.0f;
                const Point<float> xOffsetCorrection{5.0f - Shaping::yOffset - xHalfWidhtOffsetcorrection,
                                                     -yImageOffsetCorrection};

                tl = util::rotate(tl - center, verticalRotation) + center + xOffsetCorrection + verticalizedLabelOffset;
                tr = util::rotate(tr - center, verticalRotation) + center + xOffsetCorrection + verticalizedLabelOffset;
                bl = util::rotate(bl - center, verticalRotation) + center + xOffsetCorrection + verticalizedLabelOffset;
                br = util::rotate(br - center, verticalRotation) + center + xOffsetCorrection + verticalizedLabelOffset;
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

            quads.emplace_back(
                tl, tr, bl, br, rect, shapedText.writingMode, glyphOffset, isSDF, positionedGlyph.sectionIndex);
        }
    }

    return quads;
}
} // namespace mbgl
