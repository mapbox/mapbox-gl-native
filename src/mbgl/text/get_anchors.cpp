#include <mbgl/text/get_anchors.hpp>
#include <mbgl/text/check_max_angle.hpp>

#include <mbgl/util/interpolate.hpp>

#include <cmath>

namespace mbgl {

Anchors resample(const std::vector<Coordinate> &line, const float offset, const float spacing,
        const float angleWindowSize, const float maxAngle, const float labelLength, const bool continuedLine, const bool placeAtMiddle) {

    float distance = 0;
    float markedDistance = offset - spacing;

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
                  x = util::interpolate(float(a.x), float(b.x), t),
                  y = util::interpolate(float(a.y), float(b.y), t);

            if (x >= 0 && x < 4096 && y >= 0 && y < 4096) {
                Anchor anchor(std::round(x), std::round(y), angle, 0.5f, i);

                if (!angleWindowSize || checkMaxAngle(line, anchor, labelLength, angleWindowSize, maxAngle)) {
                    anchors.push_back(anchor);
                }
            }
        }

        distance += segmentDist;
    }

    if (!placeAtMiddle && !anchors.size() && !continuedLine) {
        // The first attempt at finding anchors at which labels can be placed failed.
        // Try again, but this time just try placing one anchor at the middle of the line.
        // This has the most effect for short lines in overscaled tiles, since the
        // initial offset used in overscaled tiles is calculated to align labels with positions in
        // parent tiles instead of placing the label as close to the beginning as possible.
        anchors = std::move(resample(line, distance / 2, spacing, angleWindowSize, maxAngle, labelLength, continuedLine, true));
    }

    return anchors;
}

Anchors getAnchors(const std::vector<Coordinate> &line, float spacing,
        const float maxAngle, const float textLeft, const float textRight,
        const float iconLeft, const float iconRight,
        const float glyphSize, const float boxScale, const float overscaling) {

    // Resample a line to get anchor points for labels and check that each
    // potential label passes text-max-angle check and has enough froom to fit
    // on the line.

    const float angleWindowSize = (textLeft - textRight) != 0.0f ?
        3.0f / 5.0f * glyphSize * boxScale :
        0;
    
    const float labelLength = fmax(textRight - textLeft, iconRight - iconLeft);
    
    // Is the line continued from outside the tile boundary?
    const bool continuedLine = (line[0].x == 0 || line[0].x == 4096 || line[0].y == 0 || line[0].y == 4096);
    
    // Is the label long, relative to the spacing?
    // If so, adjust the spacing so there is always a minimum space of `spacing / 4` between label edges.
    if (spacing - labelLength * boxScale  < spacing / 4) {
        spacing = labelLength * boxScale + spacing / 4;
    }
    
    // Offset the first anchor by:
    // Either half the label length plus a fixed extra offset if the line is not continued
    // Or half the spacing if the line is continued.

    // For non-continued lines, add a bit of fixed extra offset to avoid collisions at T intersections.
    const float fixedExtraOffset = glyphSize * 2;
    
    const float offset = !continuedLine ?
    std::fmod((labelLength / 2 + fixedExtraOffset) * boxScale * overscaling, spacing) :
    std::fmod(spacing / 2 * overscaling, spacing);

    return resample(line, offset, spacing, angleWindowSize, maxAngle, labelLength * boxScale, continuedLine, false);
}

}
