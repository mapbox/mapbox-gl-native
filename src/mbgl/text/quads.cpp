#include <mbgl/geometry/anchor.hpp>
#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/math/minmax.hpp>
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

constexpr const auto border = ImagePosition::padding;

float computeStretchSum(const ImageStretches& stretches) {
    float sum = 0;
    for (auto& stretch : stretches) {
        sum += stretch.second - stretch.first;
    }
    return sum;
}

float sumWithinRange(const ImageStretches& stretches, const float min, const float max) {
    float sum = 0;
    for (auto& stretch : stretches) {
        sum += util::max(min, util::min(max, stretch.second)) - util::max(min, util::min(max, stretch.first));
    }
    return sum;
}

inline float getEmOffset(float stretchOffset, float stretchSize, float iconSize, float iconOffset) {
    return iconOffset + iconSize * stretchOffset / stretchSize;
}

inline float getPxOffset(float fixedOffset, float fixedSize, float stretchOffset, float stretchSize) {
    return fixedOffset - fixedSize * stretchOffset / stretchSize;
}

struct Cut {
    float fixed;
    float stretch;
};

using Cuts = std::vector<Cut>;

Cuts stretchZonesToCuts(const ImageStretches& stretchZones, const float fixedSize, const float stretchSize) {
    Cuts cuts{{-border, 0}};

    for (auto& zone : stretchZones) {
        const auto c1 = zone.first;
        const auto c2 = zone.second;
        const auto lastStretch = cuts.back().stretch;
        cuts.emplace_back(Cut{c1 - lastStretch, lastStretch});
        cuts.emplace_back(Cut{c1 - lastStretch, lastStretch + (c2 - c1)});
    }
    cuts.emplace_back(Cut{fixedSize + border, stretchSize});
    return cuts;
}

SymbolQuads getIconQuads(const PositionedIcon& shapedIcon,
                         const float iconRotate,
                         const SymbolContent iconType,
                         const bool hasIconTextFit) {
    SymbolQuads quads;

    const ImagePosition& image = shapedIcon.image();
    const float pixelRatio = image.pixelRatio;
    const uint16_t imageWidth = image.paddedRect.w - 2 * border;
    const uint16_t imageHeight = image.paddedRect.h - 2 * border;

    const float iconWidth = shapedIcon.right() - shapedIcon.left();
    const float iconHeight = shapedIcon.bottom() - shapedIcon.top();

    const ImageStretches stretchXFull{{0, imageWidth}};
    const ImageStretches stretchYFull{{0, imageHeight}};
    const ImageStretches& stretchX = !image.stretchX.empty() ? image.stretchX : stretchXFull;
    const ImageStretches& stretchY = !image.stretchY.empty() ? image.stretchY : stretchYFull;

    const float stretchWidth = computeStretchSum(stretchX);
    const float stretchHeight = computeStretchSum(stretchY);
    const float fixedWidth = imageWidth - stretchWidth;
    const float fixedHeight = imageHeight - stretchHeight;

    float stretchOffsetX = 0;
    float stretchContentWidth = stretchWidth;
    float stretchOffsetY = 0;
    float stretchContentHeight = stretchHeight;
    float fixedOffsetX = 0;
    float fixedContentWidth = fixedWidth;
    float fixedOffsetY = 0;
    float fixedContentHeight = fixedHeight;

    if (hasIconTextFit && image.content) {
        auto& content = *image.content;
        stretchOffsetX = sumWithinRange(stretchX, 0, content.left);
        stretchOffsetY = sumWithinRange(stretchY, 0, content.top);
        stretchContentWidth = sumWithinRange(stretchX, content.left, content.right);
        stretchContentHeight = sumWithinRange(stretchY, content.top, content.bottom);
        fixedOffsetX = content.left - stretchOffsetX;
        fixedOffsetY = content.top - stretchOffsetY;
        fixedContentWidth = content.right - content.left - stretchContentWidth;
        fixedContentHeight = content.bottom - content.top - stretchContentHeight;
    }

    optional<std::array<float, 4>> matrix{nullopt};
    if (iconRotate) {
        const float angle = iconRotate * util::DEG2RAD;
        const float angle_sin = std::sin(angle);
        const float angle_cos = std::cos(angle);
        matrix = std::array<float, 4>{{angle_cos, -angle_sin, angle_sin, angle_cos}};
    }

    auto makeBox = [&](Cut left, Cut top, Cut right, Cut bottom) {
        const float leftEm =
            getEmOffset(left.stretch - stretchOffsetX, stretchContentWidth, iconWidth, shapedIcon.left());
        const float leftPx = getPxOffset(left.fixed - fixedOffsetX, fixedContentWidth, left.stretch, stretchWidth);

        const float topEm =
            getEmOffset(top.stretch - stretchOffsetY, stretchContentHeight, iconHeight, shapedIcon.top());
        const float topPx = getPxOffset(top.fixed - fixedOffsetY, fixedContentHeight, top.stretch, stretchHeight);

        const float rightEm =
            getEmOffset(right.stretch - stretchOffsetX, stretchContentWidth, iconWidth, shapedIcon.left());
        const float rightPx = getPxOffset(right.fixed - fixedOffsetX, fixedContentWidth, right.stretch, stretchWidth);

        const float bottomEm =
            getEmOffset(bottom.stretch - stretchOffsetY, stretchContentHeight, iconHeight, shapedIcon.top());
        const float bottomPx =
            getPxOffset(bottom.fixed - fixedOffsetY, fixedContentHeight, bottom.stretch, stretchHeight);

        Point<float> tl(leftEm, topEm);
        Point<float> tr(rightEm, topEm);
        Point<float> br(rightEm, bottomEm);
        Point<float> bl(leftEm, bottomEm);
        const Point<float> pixelOffsetTL(leftPx / pixelRatio, topPx / pixelRatio);
        const Point<float> pixelOffsetBR(rightPx / pixelRatio, bottomPx / pixelRatio);

        if (matrix) {
            tl = util::matrixMultiply(*matrix, tl);
            tr = util::matrixMultiply(*matrix, tr);
            bl = util::matrixMultiply(*matrix, bl);
            br = util::matrixMultiply(*matrix, br);
        }

        const float x1 = left.stretch + left.fixed;
        const float x2 = right.stretch + right.fixed;
        const float y1 = top.stretch + top.fixed;
        const float y2 = bottom.stretch + bottom.fixed;

        // TODO: consider making texture coordinates float instead of uint16_t
        const Rect<uint16_t> subRect{static_cast<uint16_t>(image.paddedRect.x + border + x1),
                                     static_cast<uint16_t>(image.paddedRect.y + border + y1),
                                     static_cast<uint16_t>(x2 - x1),
                                     static_cast<uint16_t>(y2 - y1)};

        const float minFontScaleX = fixedContentWidth / pixelRatio / iconWidth;
        const float minFontScaleY = fixedContentHeight / pixelRatio / iconHeight;

        // Icon quad is padded, so texture coordinates also need to be padded.
        quads.emplace_back(tl,
                           tr,
                           bl,
                           br,
                           subRect,
                           WritingModeType::None,
                           Point<float>{0.0f, 0.0f},
                           iconType == SymbolContent::IconSDF,
                           pixelOffsetTL,
                           pixelOffsetBR,
                           Point<float>{minFontScaleX, minFontScaleY});
    };

    if (!hasIconTextFit || (image.stretchX.empty() && image.stretchY.empty())) {
        makeBox({0, -1}, {0, -1}, {0, static_cast<float>(imageWidth + 1)}, {0, static_cast<float>(imageHeight + 1)});
    } else {
        const auto xCuts = stretchZonesToCuts(stretchX, fixedWidth, stretchWidth);
        const auto yCuts = stretchZonesToCuts(stretchY, fixedHeight, stretchHeight);

        for (size_t xi = 0; xi < xCuts.size() - 1; xi++) {
            const auto& x1 = xCuts[xi];
            const auto& x2 = xCuts[xi + 1];
            for (size_t yi = 0; yi < yCuts.size() - 1; yi++) {
                const auto& y1 = yCuts[yi];
                const auto& y2 = yCuts[yi + 1];
                makeBox(x1, y1, x2, y2);
            }
        }
    }

    return quads;
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

            Point<float> pixelOffsetTL;
            Point<float> pixelOffsetBR;
            Point<float> minFontScale;

            quads.emplace_back(tl,
                               tr,
                               bl,
                               br,
                               rect,
                               shapedText.writingMode,
                               glyphOffset,
                               isSDF,
                               pixelOffsetTL,
                               pixelOffsetBR,
                               minFontScale,
                               positionedGlyph.sectionIndex);
        }
    }

    return quads;
}
} // namespace mbgl
