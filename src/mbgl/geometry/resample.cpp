#include <mbgl/geometry/resample.hpp>

#include <mbgl/util/interpolate.hpp>

#include <cmath>

namespace mbgl {

const float minScale = 0.5f;
const std::array<std::vector<float>, 4> minScaleArrays = {{
    /*1:*/ { minScale },
    /*2:*/ { minScale, 2 },
    /*4:*/ { minScale, 4, 2, 4 },
    /*8:*/ { minScale, 8, 4, 8, 2, 8, 4, 8 }
}};


Anchors resample(const std::vector<Coordinate> &vertices, float spacing,
                 const float /*minScale*/, float maxScale, const float tilePixelRatio,
                 float offset) {

    maxScale = std::round(std::fmax(std::fmin(8.0f, maxScale / 2.0f), 1.0f));
    spacing *= tilePixelRatio / maxScale;
    offset *= tilePixelRatio;
    const size_t index = util::clamp<size_t>(std::floor(std::log(maxScale) / std::log(2)), 0, minScaleArrays.size() - 1);
    const std::vector<float> &minScales = minScaleArrays[index];
    const size_t len = minScales.size();

    float distance = 0.0f;
    float markedDistance = offset != 0.0f ? offset - spacing : offset;
    int added = 0;

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
                  x = util::interpolate(a.x, b.x, t),
                  y = util::interpolate(a.y, b.y, t),
                  s = minScales[added % len];

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
