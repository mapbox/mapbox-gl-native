#include "geojsonvt_simplify.hpp"

#include <stack>

namespace mapbox { namespace util { namespace geojsonvt {

void Simplify::simplify(ProjectedGeometryContainer &points, double tolerance) {

    const double sqTolerance = tolerance * tolerance;
    const size_t len = points.members.size();
    size_t first = 0;
    size_t last = len - 1;
    std::stack<size_t> stack;
    double maxSqDist = 0;
    double sqDist = 0;
    size_t index = 0;

    points.members[first].get<ProjectedPoint>().z = 1;
    points.members[last].get<ProjectedPoint>().z  = 1;

    while (last) {

        maxSqDist = 0;

        for (size_t i = (first + 1); i < last; ++i) {
            sqDist = getSqSegDist(points.members[i].get<ProjectedPoint>(),
                points.members[first].get<ProjectedPoint>(),
                points.members[last].get<ProjectedPoint>());

            if (sqDist > maxSqDist) {
                index = i;
                maxSqDist = sqDist;
            }
        }

        if (maxSqDist > sqTolerance) {
            points.members[index].get<ProjectedPoint>().z = maxSqDist;
            stack.push(first);
            stack.push(index);
            stack.push(index);
            stack.push(last);
        }

        if (stack.size()) {
            last = stack.top();
            stack.pop();
        } else {
            last = 0;
        }

        if (stack.size()) {
            first = stack.top();
            stack.pop();
        } else {
            first = 0;
        }
    }
}

double Simplify::getSqSegDist(const ProjectedPoint &p, const ProjectedPoint &a, const ProjectedPoint &b) {

    double x = a.x;
    double y = a.y;
    double dx = b.x - a.x;
    double dy = b.y - a.y;

    if (dx || dy) {

        const double t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / (dx * dx + dy * dy);

        if (t > 1) {
            x = b.x;
            y = b.y;
        } else if (t) {
            x += dx * t;
            y += dy * t;
        }
    }

    dx = p.x - x;
    dy = p.y - y;

    return dx * dx + dy * dy;
}

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */
