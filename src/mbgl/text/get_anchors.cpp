#include <mbgl/text/get_anchors.hpp>
#include <mbgl/text/check_max_angle.hpp>

#include <mbgl/util/interpolate.hpp>

#include <cmath>

namespace mbgl {

Anchors resample(const std::vector<Coordinate> &line, const float offset, const float spacing,
        const float angleWindowSize, const float maxAngle, const float labelLength, const bool placeAtMiddle) {

    float distance = 0;
    float markedDistance = offset != 0.0f ? offset - spacing : 0;

    Anchors anchors;

    auto end = line.end() - 1;
    int i = 0;
    for (auto it = line.begin(); it != end; it++, i++) {
        const Coordinate &a = *(it);
        const Coordinate &b = *(it + 1);

        const float segmentDist = util::dist<float>(a, b);
        const float angle = util::angle_to(b, a);

        while (markedDistance + spacing < distance + segmentDist) {
            markedDistance += spacing;

            float t = (markedDistance - distance) / segmentDist,
                  x = util::interpolate(a.x, b.x, t),
                  y = util::interpolate(a.y, b.y, t);

            if (x >= 0 && x < 4096 && y >= 0 && y < 4096) {
                Anchor anchor(x, y, angle, 0.5f, i);

                if (!angleWindowSize || checkMaxAngle(line, anchor, labelLength, angleWindowSize, maxAngle)) {
                    anchors.push_back(anchor);
                }
            }
        }

        distance += segmentDist;
    }

    if (!placeAtMiddle && !anchors.size()) {
        // The first attempt at finding anchors at which labels can be placed failed.
        // Try again, but this time just try placing one anchor at the middle of the line.
        // This has the most effect for short lines in overscaled tiles, since the
        // initial offset used in overscaled tiles is calculated to align labels with positions in
        // parent tiles instead of placing the label as close to the beginning as possible.
        anchors = std::move(resample(line, distance / 2, spacing, angleWindowSize, maxAngle, labelLength, true));
    }

    return anchors;
}

Anchors getAnchors(const std::vector<Coordinate> &line, float spacing,
        const bool maxAngle, const float left, const float right,
        const float glyphSize, const float boxScale, const float overscaling) {

    // Resample a line to get anchor points for labels and check that each
    // potential label passes text-max-angle check and has enough froom to fit
    // on the line.

    const float angleWindowSize = (left - right) != 0.0f ?
        3.0f / 5.0f * glyphSize * boxScale :
        0;

    // Offset the first anchor by half the label length (or half the spacing distance for icons).
    // Add a bit of extra offset to avoid collisions at T intersections.
    const float labelLength = right - left ? right - left : spacing;
    const float extraOffset = glyphSize * 2;
    const float offset = std::fmod((labelLength / 2 + extraOffset) * boxScale * overscaling, spacing);

    return resample(line, offset, spacing, angleWindowSize, maxAngle, labelLength * boxScale, false);
}

}
