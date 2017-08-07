#include <mbgl/text/check_max_angle.hpp>
#include <mbgl/geometry/anchor.hpp>
#include <mbgl/util/math.hpp>

#include <queue>

namespace mbgl{

struct Corner {
    Corner(float _distance, float _angleDelta) :
        distance(_distance), angleDelta(_angleDelta) {}
    float distance;
    float angleDelta;
};

bool checkMaxAngle(const GeometryCoordinates& line,
                   const Anchor& anchor,
                   const float labelLength,
                   const float windowSize,
                   const float maxAngle) {
    // horizontal labels always pass
    if (anchor.segment < 0) return true;

    GeometryCoordinate anchorPoint = convertPoint<int16_t>(anchor.point);
    GeometryCoordinate &p = anchorPoint;
    int index = anchor.segment + 1;
    float anchorDistance = 0;

    // move backwards along the line to the first segment the label appears on
    while (anchorDistance > -labelLength / 2) {
        index--;

        // there isn't enough room for the label after the beginning of the line
        if (index < 0) return false;

        anchorDistance -= util::dist<float>(line[index], p);
        p = line[index];
    }

    anchorDistance += util::dist<float>(line[index], line[index + 1]);
    index++;

    // store recent corners and their total angle difference
    std::queue<Corner> recentCorners;
    float recentAngleDelta = 0;

     // move forwards by the length of the label and check angles along the way
    while (anchorDistance < labelLength / 2) {

        // there isn't enough room for the label before the end of the line
        if (index + 1 >= (int)line.size()) return false;

        auto& prev = line[index - 1];
        auto& current = line[index];
        auto& next = line[index + 1];

        float angleDelta = util::angle_to(prev, current) - util::angle_to(current, next);
        // restrict angle to -pi..pi range
        angleDelta = std::fabs(std::fmod(angleDelta + 3 * M_PI, M_PI * 2) - M_PI);

        recentCorners.emplace(anchorDistance, angleDelta);
        recentAngleDelta += angleDelta;

        // remove corners that are far enough away from the list of recent anchors
        while (anchorDistance - recentCorners.front().distance > windowSize) {
            recentAngleDelta -= recentCorners.front().angleDelta;
            recentCorners.pop();
        }

        // the sum of angles within the window area exceeds the maximum allowed value. check fails.
        if (recentAngleDelta > maxAngle) return false;

        index++;
        anchorDistance += util::dist<float>(current, next);
    }

    // no part of the line had an angle greater than the maximum allowed. check passes.
    return true;
}

} // namespace mbgl
