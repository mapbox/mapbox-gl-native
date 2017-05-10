#include <mbgl/text/get_anchors.hpp>
#include <mbgl/text/check_max_angle.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>

#include <cassert>
#include <cmath>

namespace mbgl {

static Anchors resample(const GeometryCoordinates& line,
                        const float offset,
                        const float spacing,
                        const float angleWindowSize,
                        const float maxAngle,
                        const float labelLength,
                        const bool continuedLine,
                        const bool placeAtMiddle) {
    const float halfLabelLength = labelLength / 2.0f;
    float lineLength = 0;
    for (auto it = line.begin(), end = line.end() - 1; it != end; it++) {
        lineLength += util::dist<float>(*(it), *(it + 1));
    }

    float distance = 0;
    float markedDistance = offset - spacing;

    Anchors anchors;

    assert(spacing > 0.0);

    int i = 0;
    for (auto it = line.begin(), end = line.end() - 1; it != end; it++, i++) {
        const GeometryCoordinate& a = *(it);
        const GeometryCoordinate& b = *(it + 1);

        const auto segmentDist = util::dist<float>(a, b);
        const float angle = util::angle_to(b, a);

        while (markedDistance + spacing < distance + segmentDist) {
            markedDistance += spacing;

            float t = (markedDistance - distance) / segmentDist,
                  x = util::interpolate(float(a.x), float(b.x), t),
                  y = util::interpolate(float(a.y), float(b.y), t);

            // Check that the point is within the tile boundaries and that
            // the label would fit before the beginning and end of the line
            // if placed at this point.
            if (x >= 0 && x < util::EXTENT && y >= 0 && y < util::EXTENT &&
                    markedDistance - halfLabelLength >= 0.0f &&
                    markedDistance + halfLabelLength <= lineLength) {
                Anchor anchor(::round(x), ::round(y), angle, 0.5f, i);

                if (!angleWindowSize || checkMaxAngle(line, anchor, labelLength, angleWindowSize, maxAngle)) {
                    anchors.push_back(anchor);
                }
            }
        }

        distance += segmentDist;
    }

    if (!placeAtMiddle && anchors.empty() && !continuedLine) {
        // The first attempt at finding anchors at which labels can be placed failed.
        // Try again, but this time just try placing one anchor at the middle of the line.
        // This has the most effect for short lines in overscaled tiles, since the
        // initial offset used in overscaled tiles is calculated to align labels with positions in
        // parent tiles instead of placing the label as close to the beginning as possible.
        anchors = resample(line, distance / 2, spacing, angleWindowSize, maxAngle, labelLength, continuedLine, true);
    }

    return anchors;
}

Anchors getAnchors(const GeometryCoordinates& line,
                   float spacing,
                   const float maxAngle,
                   const float textLeft,
                   const float textRight,
                   const float iconLeft,
                   const float iconRight,
                   const float glyphSize,
                   const float boxScale,
                   const float overscaling) {
    if (line.empty()) {
        return {};
    }

    // Resample a line to get anchor points for labels and check that each
    // potential label passes text-max-angle check and has enough froom to fit
    // on the line.

    const float angleWindowSize = (textLeft - textRight) != 0.0f ?
        3.0f / 5.0f * glyphSize * boxScale :
        0;

    const float labelLength = fmax(textRight - textLeft, iconRight - iconLeft);

    // Is the line continued from outside the tile boundary?
    const bool continuedLine = (line[0].x == 0 || line[0].x == util::EXTENT || line[0].y == 0 || line[0].y == util::EXTENT);

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

} // namespace mbgl
