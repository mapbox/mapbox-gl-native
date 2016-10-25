#include <mbgl/layout/clip_lines.hpp>

#include <cmath>

namespace mbgl {
namespace util {

GeometryCollection clipLines(const GeometryCollection &lines,
        const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2) {

    GeometryCollection clippedLines;

    for (auto& line : lines) {

        if (line.empty())
            continue;

        auto end = line.end() - 1;
        for (auto it = line.begin(); it != end; it++) {
            GeometryCoordinate p0 = *(it);
            GeometryCoordinate p1 = *(it + 1);

            if (p0.x < x1 && p1.x < x1) {
                continue;
            } else if (p0.x < x1) {
                p0 = { x1, static_cast<int16_t>(::round(p0.y + (p1.y - p0.y) * ((float)(x1 - p0.x) / (p1.x - p0.x)))) };
            } else if (p1.x < x1) {
                p1 = { x1, static_cast<int16_t>(::round(p0.y + (p1.y - p0.y) * ((float)(x1 - p0.x) / (p1.x - p0.x)))) };
            }

            if (p0.y < y1 && p1.y < y1) {
                continue;
            } else if (p0.y < y1) {
                p0 = { static_cast<int16_t>(::round(p0.x + (p1.x - p0.x) * ((float)(y1 - p0.y) / (p1.y - p0.y)))), y1 };
            } else if (p1.y < y1) {
                p1 = { static_cast<int16_t>(::round(p0.x + (p1.x - p0.x) * ((float)(y1 - p0.y) / (p1.y - p0.y)))), y1 };
            }

            if (p0.x >= x2 && p1.x >= x2) {
                continue;
            } else if (p0.x >= x2) {
                p0 = { x2, static_cast<int16_t>(::round(p0.y + (p1.y - p0.y) * ((float)(x2 - p0.x) / (p1.x - p0.x)))) };
            } else if (p1.x >= x2) {
                p1 = { x2, static_cast<int16_t>(::round(p0.y + (p1.y - p0.y) * ((float)(x2 - p0.x) / (p1.x - p0.x)))) };
            }

            if (p0.y >= y2 && p1.y >= y2) {
                continue;
            } else if (p0.y >= y2) {
                p0 = { static_cast<int16_t>(::round(p0.x + (p1.x - p0.x) * ((float)(y2 - p0.y) / (p1.y - p0.y)))), y2 };
            } else if (p1.y >= y2) {
                p1 = { static_cast<int16_t>(::round(p0.x + (p1.x - p0.x) * ((float)(y2 - p0.y) / (p1.y - p0.y)))), y2 };
            }

            if (clippedLines.empty() || (!clippedLines.back().empty() && !(p0 == clippedLines.back().back()))) {
                clippedLines.emplace_back();
                clippedLines.back().push_back(p0);
            }

            clippedLines.back().push_back(p1);
        }
    }

    return clippedLines;
}

} // end namespace util
} // end namespace mbgl
