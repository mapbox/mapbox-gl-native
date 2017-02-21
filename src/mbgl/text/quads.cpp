#include <mbgl/text/quads.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/geometry/anchor.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/constants.hpp>

#include <cassert>

namespace mbgl {

using namespace style;

const float globalMinScale = 0.5f; // underscale by 1 zoom level

SymbolQuad getIconQuad(Anchor& anchor,
                       const PositionedIcon& shapedIcon,
                       const GeometryCoordinates& line,
                       const SymbolLayoutProperties::Evaluated& layout,
                       const style::SymbolPlacementType placement, 
                       const Shaping& shapedText) {
    auto image = *(shapedIcon.image);

    const float border = 1.0;
    auto left = shapedIcon.left - border;
    auto right = left + image.pos.w / image.relativePixelRatio;
    auto top = shapedIcon.top - border;
    auto bottom = top + image.pos.h / image.relativePixelRatio;
    Point<float> tl;
    Point<float> tr;
    Point<float> br;
    Point<float> bl;

    if (layout.get<IconTextFit>() != IconTextFitType::None && shapedText) {
        auto iconWidth = right - left;
        auto iconHeight = bottom - top;
        auto size = layout.get<TextSize>() / 24.0f;
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

    float angle = shapedIcon.angle;
    if (placement == style::SymbolPlacementType::Line) {
        assert(static_cast<unsigned int>(anchor.segment) < line.size());
        const GeometryCoordinate &prev= line[anchor.segment];
        if (anchor.point.y == prev.y && anchor.point.x == prev.x &&
            static_cast<unsigned int>(anchor.segment + 1) < line.size()) {
            const GeometryCoordinate &next= line[anchor.segment + 1];
            angle += std::atan2(anchor.point.y - next.y, anchor.point.x - next.x) + M_PI;
        } else {
            angle += std::atan2(anchor.point.y - prev.y, anchor.point.x - prev.x);
        }
    }

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

    return SymbolQuad { tl, tr, bl, br, image.pos, 0, 0, anchor.point, globalMinScale, std::numeric_limits<float>::infinity(), shapedText.writingMode };
}

struct GlyphInstance {
    explicit GlyphInstance(Point<float> anchorPoint_) : anchorPoint(std::move(anchorPoint_)) {}
    explicit GlyphInstance(Point<float> anchorPoint_, float offset_, float minScale_, float maxScale_,
            float angle_)
        : anchorPoint(std::move(anchorPoint_)), offset(offset_), minScale(minScale_), maxScale(maxScale_), angle(angle_) {}

    const Point<float> anchorPoint;
    const float offset = 0.0f;
    const float minScale = globalMinScale;
    const float maxScale = std::numeric_limits<float>::infinity();
    const float angle = 0.0f;
};

typedef std::vector<GlyphInstance> GlyphInstances;

void getSegmentGlyphs(std::back_insert_iterator<GlyphInstances> glyphs,
                      Anchor& anchor,
                      float offset,
                      const GeometryCoordinates& line,
                      int segment,
                      bool forward) {
    const bool upsideDown = !forward;

    if (offset < 0)
        forward = !forward;

    if (forward)
        segment++;

    assert((int)line.size() > segment);
    Point<float> end = convertPoint<float>(line[segment]);
    Point<float> newAnchorPoint = anchor.point;
    float prevscale = std::numeric_limits<float>::infinity();

    offset = std::fabs(offset);

    const float placementScale = anchor.scale;

    while (true) {
        const float dist = util::dist<float>(newAnchorPoint, end);
        const float scale = offset / dist;
        float angle = std::atan2(end.y - newAnchorPoint.y, end.x - newAnchorPoint.x);
        if (!forward)
            angle += M_PI;

        glyphs = GlyphInstance{
            /* anchor */ newAnchorPoint,
            /* offset */ static_cast<float>(upsideDown ? M_PI : 0.0),
            /* minScale */ scale,
            /* maxScale */ prevscale,
            /* angle */ static_cast<float>(std::fmod((angle + 2.0 * M_PI), (2.0 * M_PI)))};

        if (scale <= placementScale)
            break;

        newAnchorPoint = end;

        // skip duplicate nodes
        while (newAnchorPoint == end) {
            segment += forward ? 1 : -1;
            if ((int)line.size() <= segment || segment < 0) {
                anchor.scale = scale;
                return;
            }
            end = convertPoint<float>(line[segment]);
        }

        Point<float> normal = util::normal<float>(newAnchorPoint, end) * dist;
        newAnchorPoint = newAnchorPoint - normal;

        prevscale = scale;
    }
}

SymbolQuads getGlyphQuads(Anchor& anchor,
                          const Shaping& shapedText,
                          const float boxScale,
                          const GeometryCoordinates& line,
                          const SymbolLayoutProperties::Evaluated& layout,
                          const style::SymbolPlacementType placement,
                          const GlyphPositions& face) {
    const float textRotate = layout.get<TextRotate>() * util::DEG2RAD;
    const bool keepUpright = layout.get<TextKeepUpright>();

    SymbolQuads quads;

    for (const PositionedGlyph &positionedGlyph: shapedText.positionedGlyphs) {
        auto face_it = face.find(positionedGlyph.glyph);
        if (face_it == face.end())
            continue;
        const Glyph &glyph = face_it->second;
        const Rect<uint16_t> &rect = glyph.rect;

        if (!glyph)
            continue;

        if (!rect.hasArea())
            continue;

        const float centerX = (positionedGlyph.x + glyph.metrics.advance / 2.0f) * boxScale;

        GlyphInstances glyphInstances;
        if (placement == style::SymbolPlacementType::Line) {
            getSegmentGlyphs(std::back_inserter(glyphInstances), anchor, centerX, line, anchor.segment, true);
            if (keepUpright)
                getSegmentGlyphs(std::back_inserter(glyphInstances), anchor, centerX, line, anchor.segment, false);
        } else {
            glyphInstances.emplace_back(GlyphInstance{anchor.point});
        }

        // The rects have an addditional buffer that is not included in their size;
        const float glyphPadding = 1.0f;
        const float rectBuffer = 3.0f + glyphPadding;

        const float x1 = positionedGlyph.x + glyph.metrics.left - rectBuffer;
        const float y1 = positionedGlyph.y - glyph.metrics.top - rectBuffer;
        const float x2 = x1 + rect.w;
        const float y2 = y1 + rect.h;

        const Point<float> center{positionedGlyph.x, static_cast<float>(static_cast<float>(glyph.metrics.advance) / 2.0)};

        Point<float> otl{x1, y1};
        Point<float> otr{x2, y1};
        Point<float> obl{x1, y2};
        Point<float> obr{x2, y2};

        if (positionedGlyph.angle != 0) {
            otl = util::rotate(otl - center, positionedGlyph.angle) + center;
            otr = util::rotate(otr - center, positionedGlyph.angle) + center;
            obl = util::rotate(obl - center, positionedGlyph.angle) + center;
            obr = util::rotate(obr - center, positionedGlyph.angle) + center;
        }

        for (const GlyphInstance &instance : glyphInstances) {
            Point<float> tl = otl;
            Point<float> tr = otr;
            Point<float> bl = obl;
            Point<float> br = obr;

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

            // Prevent label from extending past the end of the line
            const float glyphMinScale = std::max(instance.minScale, anchor.scale);

            const float anchorAngle = std::fmod((anchor.angle + instance.offset + 2 * M_PI), (2 * M_PI));
            const float glyphAngle = std::fmod((instance.angle + instance.offset + 2 * M_PI), (2 * M_PI));
            quads.emplace_back(tl, tr, bl, br, rect, anchorAngle, glyphAngle, instance.anchorPoint, glyphMinScale, instance.maxScale, shapedText.writingMode);
        }
    }

    return quads;
}
} // namespace mbgl
