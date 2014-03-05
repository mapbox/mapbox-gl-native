#include <llmr/map/coverage.hpp>
#include <functional>
#include <cmath>

// Taken from polymaps src/Layer.js
// https://github.com/simplegeo/polymaps/blob/master/src/Layer.js#L333-L383

struct edge {
    double x0 = 0, y0 = 0;
    double x1 = 0, y1 = 0;
    double dx = 0, dy = 0;
    edge(double x0, double y0, double x1, double y1, double dx, double dy)
        : x0(x0), y0(y0), x1(x1), y1(y1), dx(dx), dy(dy) {}
};

typedef const std::function<void(int32_t, int32_t, int32_t, int32_t)> ScanLine;

// scan-line conversion
edge _edge(const llmr::vec2<double> a, const llmr::vec2<double> b) {
    if (a.y > b.y) {
        return { b.x, b.y, a.x, a.y, a.x - b.x, a.y - b.y };
    } else {
        return { a.x, a.y, b.x, b.y, b.x - a.x, b.y - a.y };
    }
}

// scan-line conversion
void _scanSpans(edge e0, edge e1, int32_t ymin, int32_t ymax, ScanLine scanLine) {
    double y0 = fmax(ymin, floor(e1.y0)),
           y1 = fmin(ymax, ceil(e1.y1));

    // sort edges by x-coordinate
    if ((e0.x0 == e1.x0 && e0.y0 == e1.y0) ?
        (e0.x0 + e1.dy / e0.dy * e0.dx < e1.x1) :
        (e0.x1 - e1.dy / e0.dy * e0.dx < e1.x0)) {
        std::swap(e0, e1);
    }

    // scan lines!
    double m0 = e0.dx / e0.dy,
           m1 = e1.dx / e1.dy,
           d0 = e0.dx > 0, // use y + 1 to compute x0
           d1 = e1.dx < 0; // use y + 1 to compute x1
    for (int32_t y = y0; y < y1; y++) {
        double x0 = m0 * fmax(0, fmin(e0.dy, y + d0 - e0.y0)) + e0.x0,
            x1 = m1 * fmax(0, fmin(e1.dy, y + d1 - e1.y0)) + e1.x0;
        scanLine(floor(x1), ceil(x0), y, ymax);
    }
}

// scan-line conversion
void _scanTriangle(const llmr::vec2<double> a, const llmr::vec2<double> b, const llmr::vec2<double> c, int32_t ymin, int32_t ymax, ScanLine& scanLine) {
    edge ab = _edge(a, b);
    edge bc = _edge(b, c);
    edge ca = _edge(c, a);

    // sort edges by y-length
    if (ab.dy > bc.dy) { std::swap(ab, bc); }
    if (ab.dy > ca.dy) { std::swap(ab, ca); }
    if (bc.dy > ca.dy) { std::swap(bc, ca); }

    // scan span! scan span!
    if (ab.dy) _scanSpans(ca, ab, ymin, ymax, scanLine);
    if (bc.dy) _scanSpans(ca, bc, ymin, ymax, scanLine);
}

std::forward_list<llmr::vec3<int32_t>> llmr::covering_tiles(int32_t zoom, const box& points) {
    int32_t dim = pow(2, zoom);
    std::forward_list<vec3<int32_t>> tiles;

    auto scanLine = [&tiles, zoom](int32_t x0, int32_t x1, int32_t y, int32_t ymax) {
        int32_t x;
        if (y >= 0 && y <= ymax) {
            for (x = x0; x < x1; x++) {
                tiles.emplace_front(x, y, zoom);
            }
        }
    };

    // Divide the screen up in two triangles and scan each of them:
    // \---+
    // | \ |
    // +---\.
    _scanTriangle(points.tl, points.tr, points.br, 0, dim, scanLine);
    _scanTriangle(points.br, points.bl, points.tl, 0, dim, scanLine);

    const vec2<double>& center = points.center;
    tiles.sort([&center](const vec3<int32_t>& a, const vec3<int32_t>& b) {
        // Sorts by distance from the box center
        return fabs(a.x - center.x) + fabs(a.y - center.y) <
               fabs(b.x - center.x) + fabs(b.y - center.y);
    });

    tiles.unique();

    return tiles;
}
