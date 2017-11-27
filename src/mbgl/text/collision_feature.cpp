#include <mbgl/text/collision_feature.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

CollisionFeature::CollisionFeature(const GeometryCoordinates& line,
                                   const Anchor& anchor,
                                   const float top,
                                   const float bottom,
                                   const float left,
                                   const float right,
                                   const float boxScale,
                                   const float padding,
                                   const style::SymbolPlacementType placement,
                                   IndexedSubfeature indexedFeature_,
                                   const float overscaling)
        : indexedFeature(std::move(indexedFeature_))
        , alongLine(placement == style::SymbolPlacementType::Line) {
    if (top == 0 && bottom == 0 && left == 0 && right == 0) return;

    const float y1 = top * boxScale - padding;
    const float y2 = bottom * boxScale + padding;
    const float x1 = left * boxScale - padding;
    const float x2 = right * boxScale + padding;

    if (alongLine) {
        float height = y2 - y1;
        const double length = x2 - x1;

        if (height <= 0.0f) return;

        height = std::max(10.0f * boxScale, height);

        GeometryCoordinate anchorPoint = convertPoint<int16_t>(anchor.point);
        bboxifyLabel(line, anchorPoint, anchor.segment, length, height, overscaling);
    } else {
        boxes.emplace_back(anchor.point, Point<float>{ 0, 0 }, x1, y1, x2, y2);
    }
}

void CollisionFeature::bboxifyLabel(const GeometryCoordinates& line, GeometryCoordinate& anchorPoint,
                                    const int segment, const float labelLength, const float boxSize, const float overscaling) {
    const float step = boxSize / 2;
    const int nBoxes = std::floor(labelLength / step);
    // We calculate line collision circles out to 300% of what would normally be our
    // max size, to allow collision detection to work on labels that expand as
    // they move into the distance
    // Vertically oriented labels in the distant field can extend past this padding
    // This is a noticeable problem in overscaled tiles where the pitch 0-based
    // symbol spacing will put labels very close together in a pitched map.
    // To reduce the cost of adding extra collision circles, we slowly increase
    // them for overscaled tiles.
    const float overscalingPaddingFactor = 1 + .4 * std::log(overscaling) / std::log(2);
    const int nPitchPaddingBoxes = std::floor(nBoxes * overscalingPaddingFactor / 2);

    // offset the center of the first box by half a box so that the edge of the
    // box is at the edge of the label.
    const float firstBoxOffset = -boxSize / 2;

    GeometryCoordinate &p = anchorPoint;
    int index = segment + 1;
    float anchorDistance = firstBoxOffset;
    const float labelStartDistance = -labelLength / 2;
    const float paddingStartDistance = labelStartDistance - labelLength / 8;

    // move backwards along the line to the first segment the label appears on
    do {
        index--;

        if (index < 0) {
            if (anchorDistance > labelStartDistance) {
                // there isn't enough room for the label after the beginning of the line
                // checkMaxAngle should have already caught this
                return;
            } else {
                // The line doesn't extend far enough back for all of our padding,
                // but we got far enough to show the label under most conditions.
                index = 0;
                break;
            }
        }

        anchorDistance -= util::dist<float>(line[index], p);
        p = line[index];
    } while (anchorDistance > paddingStartDistance);

    auto segmentLength = util::dist<float>(line[index], line[index + 1]);

    for (int i = -nPitchPaddingBoxes; i < nBoxes + nPitchPaddingBoxes; i++) {
        // the distance the box will be from the anchor
        const float boxOffset = i * step;
        float boxDistanceToAnchor = labelStartDistance + boxOffset;

         // make the distance between pitch padding boxes bigger
         if (boxOffset < 0) boxDistanceToAnchor += boxOffset;
         if (boxOffset > labelLength) boxDistanceToAnchor += boxOffset - labelLength;

        if (boxDistanceToAnchor < anchorDistance) {
            // The line doesn't extend far enough back for this box, skip it
            // (This could allow for line collisions on distant tiles)
            continue;
        }

        // the box is not on the current segment. Move to the next segment.
        while (anchorDistance + segmentLength < boxDistanceToAnchor) {
            anchorDistance += segmentLength;
            index++;

            // There isn't enough room before the end of the line.
            if (index + 1 >= (int)line.size()) return;

            segmentLength = util::dist<float>(line[index], line[index + 1]);
        }

        // the distance the box will be from the beginning of the segment
        const float segmentBoxDistance = boxDistanceToAnchor - anchorDistance;

        const auto& p0 = line[index];
        const auto& p1 = line[index + 1];

        Point<float> boxAnchor = {
            p0.x + segmentBoxDistance / segmentLength * (p1.x - p0.x),
            p0.y + segmentBoxDistance / segmentLength * (p1.y - p0.y)
        };
        
        // If the box is within boxSize of the anchor, force the box to be used
        // (so even 0-width labels use at least one box)
        // Otherwise, the .8 multiplication gives us a little bit of conservative
        // padding in choosing which boxes to use (see CollisionIndex#placedCollisionCircles)
        const float paddedAnchorDistance = std::abs(boxDistanceToAnchor - firstBoxOffset) < step ?
            0 :
            (boxDistanceToAnchor - firstBoxOffset) * 0.8;

        boxes.emplace_back(boxAnchor, boxAnchor - convertPoint<float>(anchorPoint), -boxSize / 2, -boxSize / 2, boxSize / 2, boxSize / 2, paddedAnchorDistance, boxSize / 2);
    }
}


} // namespace mbgl
