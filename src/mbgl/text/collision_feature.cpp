#include <mbgl/text/collision_feature.hpp>

namespace mbgl {

CollisionFeature::CollisionFeature(const std::vector<Coordinate> &line, const Anchor &anchor,
        const float top, const float bottom, const float left, const float right,
        const float boxScale, const float padding, const bool alongLine) {

    const float y1 = top * boxScale - padding;
    const float y2 = bottom * boxScale + padding;
    const float x1 = left * boxScale - padding;
    const float x2 = right * boxScale + padding;

    if (alongLine) {
        const float height = y2 - y1;
        const float length = x2 - x1;

        bboxifyLabel(line, anchor, length, height);
    } else {
        boxes.emplace_back(anchor, x1, y1, x2, y2, std::numeric_limits<float>::infinity());
    }
}

void CollisionFeature::bboxifyLabel(const std::vector<Coordinate> &line,
        const Anchor &anchor, const float length, const float height) {

    if (length && height && line.size() && anchor.x) {
        fprintf(stderr, "");
    }
}


}
