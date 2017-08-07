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
                                   const AlignmentType alignment)
        : indexedFeature(std::move(indexedFeature_)) {
    if (top == 0 && bottom == 0 && left == 0 && right == 0) return;

    const float y1 = top * boxScale - padding;
    const float y2 = bottom * boxScale + padding;
    const float x1 = left * boxScale - padding;
    const float x2 = right * boxScale + padding;

    if (placement == style::SymbolPlacementType::Line) {
        float height = y2 - y1;
        const double length = x2 - x1;

        if (height <= 0.0f) return;

        height = std::max(10.0f * boxScale, height);

        GeometryCoordinate anchorPoint = convertPoint<int16_t>(anchor.point);

        if (alignment == AlignmentType::Straight) {
            // used for icon labels that are aligned with the line, but don't curve along it
            const GeometryCoordinate vector = convertPoint<int16_t>(util::unit(convertPoint<double>(line[anchor.segment + 1] - line[anchor.segment])) * length);
            const GeometryCoordinates newLine({ anchorPoint - vector, anchorPoint + vector });
            bboxifyLabel(newLine, anchorPoint, 0, length, height);
        } else {
            // used for text labels that curve along a line
            bboxifyLabel(line, anchorPoint, anchor.segment, length, height);
        }
    } else {
        boxes.emplace_back(anchor.point, Point<float>{ 0, 0 }, x1, y1, x2, y2, std::numeric_limits<float>::infinity());
    }
}

void CollisionFeature::bboxifyLabel(const GeometryCoordinates& line, GeometryCoordinate& anchorPoint,
                                    const int segment, const float labelLength, const float boxSize) {
    const float step = boxSize / 2;
    const int nBoxes = std::floor(labelLength / step);
    // We calculate line collision boxes out to 300% of what would normally be our
    // max size, to allow collision detection to work on labels that expand as
    // they move into the distance
    const int nPitchPaddingBoxes = std::floor(nBoxes / 2);

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

        // Distance from label anchor point to inner (towards center) edge of this box
        // The tricky thing here is that box positioning doesn't change with scale,
        // but box size does change with scale.
        // Technically, distanceToInnerEdge should be:
        // Math.max(Math.abs(boxDistanceToAnchor - firstBoxOffset) - (step / scale), 0);
        // But using that formula would make solving for maxScale more difficult, so we
        // approximate with scale=2.
        // This makes our calculation spot-on at scale=2, and on the conservative side for
        // lower scales
        const float distanceToInnerEdge = std::max(std::fabs(boxDistanceToAnchor - firstBoxOffset) - step / 2, 0.0f);
        float maxScale = util::division(labelLength / 2, distanceToInnerEdge, std::numeric_limits<float>::infinity());

        // The box maxScale calculations are designed to be conservative on collisions in the scale range
        // [1,2]. At scale=1, each box has 50% overlap, and at scale=2, the boxes are lined up edge
        // to edge (beyond scale 2, gaps start to appear, which could potentially allow missed collisions).
        // We add "pitch padding" boxes to the left and right to handle effective underzooming
        // (scale < 1) when labels are in the distance. The overlap approximation could cause us to use
        // these boxes when the scale is greater than 1, but we prevent that because we know
        // they're only necessary for scales less than one.
        // This preserves the pre-pitch-padding behavior for unpitched maps.
        if (i < 0 || i >= nBoxes) {
            maxScale = std::min(maxScale, 0.99f);
        }

        boxes.emplace_back(boxAnchor, boxAnchor - convertPoint<float>(anchorPoint), -boxSize / 2, -boxSize / 2, boxSize / 2, boxSize / 2, maxScale);
    }
}

float CollisionBox::adjustedMaxScale(const std::array<float, 4>& rotationMatrix, const float yStretch) const {
    // When the map is pitched the distance covered by a line changes.
    // Adjust the max scale by (approximatePitchedLength / approximateRegularLength)
    // to compensate for this.
    const Point<float> rotatedOffset = util::matrixMultiply(rotationMatrix, offset);
    const float xSqr = rotatedOffset.x * rotatedOffset.x;
    const float ySqr = rotatedOffset.y * rotatedOffset.y;
    const float yStretchSqr = ySqr * yStretch * yStretch;
    const float adjustmentFactor = xSqr + ySqr != 0 ?
        std::sqrt((xSqr + yStretchSqr) / (xSqr + ySqr)) :
        1.0f;
    return maxScale * adjustmentFactor;
}

} // namespace mbgl
