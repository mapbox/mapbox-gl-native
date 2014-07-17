#include <mbgl/geometry/interpolate.hpp>

namespace mbgl {

Anchors interpolate(const std::vector<Coordinate> &vertices, float spacing,
                    float minScale, int start) {
    float distance = 0.0f;
    float markedDistance = 0.0f;
    int added = start;

    Anchors points;

    auto end = vertices.end() - 1;
    int i = 0;
    for (auto it = vertices.begin(); it != end; it++, i++) {
        const Coordinate &a = *(it), b = *(it + 1);

        float segmentDist = util::dist<float>(a, b);
        float angle = util::angle_to(b, a);

        while (markedDistance + spacing < distance + segmentDist) {
            markedDistance += spacing;

            float t = (markedDistance - distance) / segmentDist,
                  x = util::interp(a.x, b.x, t), y = util::interp(a.y, b.y, t),
                  s = added % 8 == 0 ? minScale : added % 4 == 0
                                                      ? 2
                                                      : added % 2 == 0 ? 4 : 8;

            if (x >= 0 && x < 4096 && y >= 0 && y < 4096) {
                points.emplace_back(x, y, angle, s, i);
            }

            added++;
        }

        distance += segmentDist;
    }

    return points;
}
}
