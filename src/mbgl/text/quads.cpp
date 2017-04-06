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

const float globalMinScale = 0.5f; // underscale by 1 zoom level

SymbolQuad getIconQuad(const Anchor& anchor,
                       const PositionedIcon& shapedIcon,
                       const GeometryCoordinates& line,
                       const SymbolLayoutProperties::Evaluated& layout,
                       const float layoutTextSize,
                       const style::SymbolPlacementType placement, 
                       const Shaping& shapedText) {
    auto image = *shapedIcon.image();

    const float border = 1.0;
    auto left = shapedIcon.left() - border;
    auto right = left + image.pos.w / image.relativePixelRatio;
    auto top = shapedIcon.top() - border;
    auto bottom = top + image.pos.h / image.relativePixelRatio;
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

    float angle = shapedIcon.angle();
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
    explicit GlyphInstance(Point<float> anchorPoint_, bool upsideDown_, float minScale_, float maxScale_,
            float angle_)
        : anchorPoint(std::move(anchorPoint_)), upsideDown(upsideDown_), minScale(minScale_), maxScale(maxScale_), angle(angle_) {}

    const Point<float> anchorPoint;
    const bool upsideDown = false;
    const float minScale = globalMinScale;
    const float maxScale = std::numeric_limits<float>::infinity();
    const float angle = 0.0f;
};

typedef std::vector<GlyphInstance> GlyphInstances;
    
struct VirtualSegment {
    Point<float> anchor;
    Point<float> end;
    size_t index;
    float minScale;
    float maxScale;
};
    
inline void insertSegmentGlyph(std::back_insert_iterator<GlyphInstances> glyphs,
                        const VirtualSegment& virtualSegment,
                        const bool glyphIsLogicallyForward,
                        const bool upsideDown) {
    float segmentAngle = std::atan2(virtualSegment.end.y - virtualSegment.anchor.y, virtualSegment.end.x - virtualSegment.anchor.x);
    // If !glyphIsLogicallyForward, we're iterating through the segments in reverse logical order as well, so we need to flip the segment angle
    float glyphAngle = glyphIsLogicallyForward ? segmentAngle : segmentAngle + M_PI;
    
    // Insert a glyph rotated at this angle for display in the range from [scale, previous(larger) scale].
    glyphs = GlyphInstance{
        /* anchor */ virtualSegment.anchor,
        /* upsideDown */ upsideDown,
        /* minScale */ virtualSegment.minScale,
        /* maxScale */ virtualSegment.maxScale,
        /* angle */ static_cast<float>(std::fmod((glyphAngle + 2.0 * M_PI), (2.0 * M_PI)))};
}

/**
 Given the distance along the line from the label anchor to the beginning of the current segment,
 project a "virtual anchor" point at the same distance along the line extending out from this segment.
 
                B <-- beginning of current segment
* . . . . . . . *--------* E <-- end of current segment
VA              |      
                /        VA = "virtual segment anchor"
               /
     ---*-----`
        A = label anchor
 
 Distance _along line_ from A to B == straight-line distance from VA to B.
 */
inline Point<float> getVirtualSegmentAnchor(const Point<float>& segmentBegin, const Point<float>& segmentEnd, float distanceFromAnchorToSegmentBegin) {
    Point<float> segmentDirectionUnitVector = util::normal<float>(segmentBegin, segmentEnd);
    return segmentBegin - (segmentDirectionUnitVector * distanceFromAnchorToSegmentBegin);
}

/*
 Given the segment joining `segmentAnchor` and `segmentEnd` and a desired offset
 `glyphDistanceFromAnchor` at which a glyph is to be placed, calculate the minimum
 "scale" at which the glyph will fall on the segment (i.e., not past the end)

 "Scale" here refers to the ratio between the *rendered* zoom level and the text-layout
 zoom level, which is 1 + (source tile's zoom level).  `glyphDistanceFromAnchor`, although
 passed in units consistent with the text-layout zoom level, is based on text size.  So
 when the tile is being rendered at z < text-layout zoom, the glyph's actual distance from
 the anchor is larger relative to the segment's length than at layout time:
 
 
                                                                   GLYPH
 z == layout-zoom, scale == 1:        segmentAnchor *--------------^-------------* segmentEnd
 z == layout-zoom - 1, scale == 0.5:  segmentAnchor *--------------^* segmentEnd
 
                                                    <-------------->
                                                    Anchor-to-glyph distance stays visually fixed,
                                                    so it changes relative to the segment.
*/
inline float getMinScaleForSegment(const float glyphDistanceFromAnchor,
                         const Point<float>& segmentAnchor,
                         const Point<float>& segmentEnd) {
    const float distanceFromAnchorToEnd = util::dist<float>(segmentAnchor, segmentEnd);
    return glyphDistanceFromAnchor / distanceFromAnchorToEnd;
}

inline Point<float> getSegmentEnd(const bool glyphIsLogicallyForward,
                           const GeometryCoordinates& line,
                           const size_t segmentIndex) {
    return convertPoint<float>(glyphIsLogicallyForward ? line[segmentIndex+1] : line[segmentIndex]);
}

optional<VirtualSegment> getNextVirtualSegment(const VirtualSegment& previousVirtualSegment,
                                               const GeometryCoordinates& line,
                                               const float glyphDistanceFromAnchor,
                                               const bool glyphIsLogicallyForward) {
    auto nextSegmentBegin = previousVirtualSegment.end;
    
    auto end = nextSegmentBegin;
    size_t index = previousVirtualSegment.index;

    // skip duplicate nodes
    while (end == nextSegmentBegin) {
        // look ahead by 2 points in the line because the segment index refers to the beginning
        // of the segment, and we need an endpoint too
        if (glyphIsLogicallyForward && (index + 2 < line.size())) {
            index += 1;
        } else if (!glyphIsLogicallyForward && index != 0) {
            index -= 1;
        } else {
            return {};
        }
        
        end = getSegmentEnd(glyphIsLogicallyForward, line, index);
    }
    
    const auto anchor = getVirtualSegmentAnchor(nextSegmentBegin, end,
                                                util::dist<float>(previousVirtualSegment.anchor,
                                                                  previousVirtualSegment.end));
    return VirtualSegment {
        anchor,
        end,
        index,
        getMinScaleForSegment(glyphDistanceFromAnchor, anchor, end),
        previousVirtualSegment.minScale
    };
}
    
/*
 Given (1) a glyph positioned relative to an anchor point and (2) a line to follow,
 calculates which segment of the line the glyph will fall on for each possible
 scale range, and for each range produces a "virtual" anchor point and an angle that will
 place the glyph on the right segment and rotated to the correct angle.
 
 Because one glyph quad is made ahead of time for each possible orientation, the
 symbol_sdf shader can quickly handle changing layout as we zoom in and out
 
 If the "keepUpright" property is set, we call getLineGlyphs twice (once upright and 
 once "upside down"). This will generate two sets of glyphs following the line in opposite
 directions. Later, SymbolLayout::place will look at the glyphs and based on the placement
 angle determine if their original anchor was "upright" or not -- based on that, it throws
 away one set of glyphs or the other (this work has to be done in the CPU, but it's just a
 filter so it's fast)
 */
void getLineGlyphs(std::back_insert_iterator<GlyphInstances> glyphs,
                      Anchor& anchor,
                      float glyphHorizontalOffsetFromAnchor,
                      const GeometryCoordinates& line,
                      size_t anchorSegment,
                      bool upsideDown) {
    assert(line.size() > anchorSegment+1);
    
    // This is true if the glyph is "logically forward" of the anchor point, based on the ordering of line segments
    //  The actual angle of the line is irrelevant
    //  If "upsideDown" is set, everything is flipped
    const bool glyphIsLogicallyForward = (glyphHorizontalOffsetFromAnchor >= 0) ^ upsideDown;
    const float glyphDistanceFromAnchor = std::fabs(glyphHorizontalOffsetFromAnchor);
    
    const auto initialSegmentEnd = getSegmentEnd(glyphIsLogicallyForward, line, anchorSegment);
    VirtualSegment virtualSegment = {
        anchor.point,
        initialSegmentEnd,
        anchorSegment,
        getMinScaleForSegment(glyphDistanceFromAnchor, anchor.point, initialSegmentEnd),
        std::numeric_limits<float>::infinity()
    };
    
    while (true) {
        insertSegmentGlyph(glyphs,
                           virtualSegment,
                           glyphIsLogicallyForward,
                           upsideDown);
        
        if (virtualSegment.minScale <= anchor.scale) {
            // No need to calculate below the scale where the label starts showing
            return;
        }
        
        optional<VirtualSegment> nextVirtualSegment = getNextVirtualSegment(virtualSegment,
                                                                            line,
                                                                            glyphDistanceFromAnchor,
                                                                            glyphIsLogicallyForward);
        if (!nextVirtualSegment) {
            // There are no more segments, so we can't fit this glyph on the line at a lower scale
            // This implies we can't show the label at all at lower scale, so we update the anchor's min scale
            anchor.scale = virtualSegment.minScale;
            return;
        } else {
            virtualSegment = *nextVirtualSegment;
        }
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
        if (face_it == face.end() || !face_it->second || !(*face_it->second).rect.hasArea())
            continue;

        const Glyph& glyph = *face_it->second;
        const Rect<uint16_t>& rect = glyph.rect;
        const float centerX = (positionedGlyph.x + glyph.metrics.advance / 2.0f) * boxScale;

        GlyphInstances glyphInstances;
        if (placement == style::SymbolPlacementType::Line) {
            getLineGlyphs(std::back_inserter(glyphInstances), anchor, centerX, line, anchor.segment, false);
            if (keepUpright)
                getLineGlyphs(std::back_inserter(glyphInstances), anchor, centerX, line, anchor.segment, true);
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

            // All the glyphs for a label are tagged with either the "right side up" or "upside down" anchor angle,
            //  which is used at placement time to determine which set to show
            const float anchorAngle = std::fmod((anchor.angle + (instance.upsideDown ? M_PI : 0.0) + 2 * M_PI), (2 * M_PI));
            const float glyphAngle = std::fmod((instance.angle + (instance.upsideDown ? M_PI : 0.0) + 2 * M_PI), (2 * M_PI));
            quads.emplace_back(tl, tr, bl, br, rect, anchorAngle, glyphAngle, instance.anchorPoint, glyphMinScale, instance.maxScale, shapedText.writingMode);
        }
    }

    return quads;
}
} // namespace mbgl
