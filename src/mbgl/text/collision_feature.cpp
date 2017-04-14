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
        boxes.emplace_back(anchor.point, x1, y1, x2, y2, std::numeric_limits<float>::infinity());
    }
}

void CollisionFeature::bboxifyLabel(const GeometryCoordinates& line, GeometryCoordinate& anchorPoint,
                                    const int segment, const float labelLength, const float boxSize) {
    const float step = boxSize / 2;
    const unsigned int nBoxes = std::floor(labelLength / step);

    // offset the center of the first box by half a box so that the edge of the
    // box is at the edge of the label.
    const float firstBoxOffset = -boxSize / 2;

    GeometryCoordinate &p = anchorPoint;
    int index = segment + 1;
    float anchorDistance = firstBoxOffset;

    // move backwards along the line to the first segment the label appears on
    do {
        index--;

        // there isn't enough room for the label after the beginning of the line
        // checkMaxAngle should have already caught this
        if (index < 0) return;

        anchorDistance -= util::dist<float>(line[index], p);
        p = line[index];
    } while (anchorDistance > -labelLength / 2);

    float segmentLength = util::dist<float>(line[index], line[index + 1]);

    for (unsigned int i = 0; i < nBoxes; i++) {
        // the distance the box will be from the anchor
        const float boxDistanceToAnchor = -labelLength / 2 + i * step;

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

        const float distanceToInnerEdge = std::max(std::fabs(boxDistanceToAnchor - firstBoxOffset) - step / 2, 0.0f);
        const float maxScale = labelLength / 2 / distanceToInnerEdge;

        boxes.emplace_back(boxAnchor, -boxSize / 2, -boxSize / 2, boxSize / 2, boxSize / 2, maxScale);
    }
}

} // namespace mbgl
