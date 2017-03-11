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

SymbolQuad getIconQuad(const Anchor& anchor,
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
    
/*
 Given (1) a glyph positioned relative to an anchor point and (2) a line to follow,
 calculates which segment of the line the glyph will fall on for each possible
 scale range, and for each range produces a "virtual" anchor point and an angle that will
 place the glyph on the right segment and rotated to the correct angle.
 
 Because one glyph quad is made ahead of time for each possible orientation, the
 symbol_sdf shader can quickly handle changing layout as we zoom in and out
 
 If "forward" is true, text is laid out left-to-right based on the logical order
 of the line, otherwise order is inverted. Logical order of the line may not have any
 obvious relationship to "left" or "right" (imagine a curvy line running from north
 to south). To handle this, if the "keepUpright" property is set, we call
 getSegmentGlyphs twice (once upright and once "upside down"). This will generate two
 sets of glyphs following the line in the opposite direction. Later, SymbolLayout::place
 will look at the glyphs and based on the placement angle determine if their original
 anchor was "upright" or not -- based on that, it throws away one set of glyphs or the
 other (this work has to be done in the CPU, but it's just a filter so it's fast)
 
 */

void getSegmentGlyphs(std::back_insert_iterator<GlyphInstances> glyphs,
                      Anchor& anchor, // Anchor point in tile coordinates
                      float offset, // "Horizontal" distance of the middle of this glyph from the anchor point, in tile coordinates (horizontal = along line segment the anchor is on)
                      const GeometryCoordinates& line, // Set of line segments for us to follow
                      int segment, // Segment we start on
                      bool forward) { // Iterate forward on segments (or if false, iterate towards earlier segments)
    const bool upsideDown = !forward; // "upside down" means we iterate along this line in the opposite direction and rotate each glyph 180 degrees
    
    if (offset < 0) // Offset < 0 -> With a centered label, roughly the first half of glyphs in the label will have offset < 0
        forward = !forward; // Since this glyph is to the left of the anchor, we'll have to move to segments to the left (aka "before") of the starting segment as the label scales up
    
    if (forward)
        segment++; // "line[segment++]" means beginning of next segment/end of segment anchor is on. If we're going backwards than "line[segment]" is the beginning of the segment the anchor is on -- and that's the "end" of the segment we start laying out on.
    
    // 0     1       2     (Segments)
    // |-----|---A---|------| (A = "anchor")
    //     This is my label
    
    assert((int)line.size() > segment);
    Point<float> end = convertPoint<float>(line[segment]); // End of segment that anchor is on
    Point<float> newAnchorPoint = anchor.point; // Start at actual anchor point
    float prevscale = std::numeric_limits<float>::infinity(); // Start at largest possible scale
    
    offset = std::fabs(offset); // If offset is negative, we flipped "forward" (and set "end" to be the left side of our starting segment), so that takes care of direction for us, we only care about magnitude
    
    const float placementScale = anchor.scale;
    
    // Starting at largest scale, find minimum scale we can still show this glyph on the current segment, then move to next segment until we hit the minimum scale for this anchor (or run out of space on the line)
    while (true) {
        // newAnchorPoint = "virtual" anchor point for current segment (may be to left or right of the actual segment)
        // end = end point of current segment
        
        // Distance from the virtual anchor point to the end of the segment. So if "dist" = 50, and "offset" = 40, the glyph fits on the segment
        // If "offset" = 60, the glyph doesn't fit on the segment
        const float dist = util::dist<float>(newAnchorPoint, end);
        
        const float scale = offset / dist; // minimum scale at which this glyph can fit on this segment
        // The way to think about scale starting at infinity is that infinitely zoomed in, each segment will be infinitely wide, and thus have space for all glyphs.
        
        float angle = std::atan2(end.y - newAnchorPoint.y, end.x - newAnchorPoint.x); // Angle of this line segment (although this is derived from the virtual anchor point, the anchor point should be along the same line as the line segment)
        if (!forward) // Rotate glyph 180 -- note that 'angle' calculation above will be implicitly rotated 180 if we're going "backwards"
            angle += M_PI;
        
        // Insert a glyph rotated at this angle for display in the range from [scale, previous(larger) scale].
        glyphs = GlyphInstance{
            /* anchor */ newAnchorPoint, // Draw relative to this point (which is somewhere on the line you would get by extending this line segment out to infinity in both directions)
            /* offset */ static_cast<float>(upsideDown ? M_PI : 0.0), // This is a different 'offset' -- it just means rotate the glyph 180 if we're in "upside down" mode
            /* minScale */ scale, // minimum scale this can show at
            /* maxScale */ prevscale, // maximum scale this version can show at
            /* angle */ static_cast<float>(std::fmod((angle + 2.0 * M_PI), (2.0 * M_PI)))}; // Orientation of the line segment we're on
        
        if (scale <= placementScale) // If the anchor isn't going to show past this scale, don't make any more calculations
            break;
        
        // OK, we finished making the glyph for that segment, now let's move on to the next segment.
        
        newAnchorPoint = end; // Start by moving our virtual anchor point to the end of this segment (aka beginning of next segment)
        
        // skip duplicate nodes
        while (newAnchorPoint == end) { // Always true on first iteration, only true after that if there are zero-length segments
            segment += forward ? 1 : -1; // Move one segment forward or backwards
            if ((int)line.size() <= segment || segment < 0) { // If we've iterated off the end of the segments for this line, then we can't handle any smaller scale
                anchor.scale = scale; // Since we can't fit one glyph of the label onto the line, that means we can't show any of the label. Update the anchor with the new minScale
                return;
            }
            end = convertPoint<float>(line[segment]); // End should now be the end of the "next" segment
        }
        
        // Imagine:
        // Prior segement = 1
        //  prior anchor = (0,30)
        //  prior end = (20,30)
        // Current segment = 2 (a 14.14 unit line at a 45 degree angle upwards)
        //  newAnchorPoint = (20,30)
        //  end = (30,40)
        //  "dist" = 20 (based on prior anchor and prior end)
        //  util::normal( (20,30), (30,40) ) = (30-20) / 14.14, (40-30) / 14.14 = .707, .707 (unit vector for direction: 45%)
        //   Anand and I (Chris) do not understand the nomenclature here -- it doesn't seem like a normal vector
        //   normal = unit vector*distance = (.707,.707)*20 = ( 14.14, 14.14 )
        Point<float> normal = util::normal<float>(newAnchorPoint, end) * dist;
        // New "virtual" anchor point: extended "out" (could be left or right depending on direction we're iterating) from the edge of this line segment by "dist".
        // As we iterate over segments, "dist" keeps growing (by the length of each statement), so that the virtual anchor point ends up further off the actual line
        newAnchorPoint = newAnchorPoint - normal;
        
        prevscale = scale; // Make minScale for previous segment is maxScale for the next segment
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
