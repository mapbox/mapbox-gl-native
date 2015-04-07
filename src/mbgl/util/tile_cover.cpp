#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/util/box.hpp>

namespace mbgl {

// Taken from polymaps src/Layer.js
// https://github.com/simplegeo/polymaps/blob/master/src/Layer.js#L333-L383

struct edge {
    double x0 = 0, y0 = 0;
    double x1 = 0, y1 = 0;
    double dx = 0, dy = 0;

    edge(vec2<double> a, vec2<double> b) {
        if (a.y > b.y) { std::swap(a, b); }
        x0 = a.x;
        y0 = a.y;
        x1 = b.x;
        y1 = b.y;
        dx = b.x - a.x;
        dy = b.y - a.y;
    }
};

typedef const std::function<void(int32_t x0, int32_t x1, int32_t y)> ScanLine;

// scan-line conversion
static void scanSpans(edge e0, edge e1, int32_t ymin, int32_t ymax, ScanLine scanLine) {
    double y0 = std::fmax(ymin, std::floor(e1.y0));
    double y1 = std::fmin(ymax, std::ceil(e1.y1));

    // sort edges by x-coordinate
    if ((e0.x0 == e1.x0 && e0.y0 == e1.y0) ?
        (e0.x0 + e1.dy / e0.dy * e0.dx < e1.x1) :
        (e0.x1 - e1.dy / e0.dy * e0.dx < e1.x0)) {
        std::swap(e0, e1);
    }

    // scan lines!
    double m0 = e0.dx / e0.dy;
    double m1 = e1.dx / e1.dy;
    double d0 = e0.dx > 0; // use y + 1 to compute x0
    double d1 = e1.dx < 0; // use y + 1 to compute x1
    for (int32_t y = y0; y < y1; y++) {
        double x0 = m0 * std::fmax(0, std::fmin(e0.dy, y + d0 - e0.y0)) + e0.x0;
        double x1 = m1 * std::fmax(0, std::fmin(e1.dy, y + d1 - e1.y0)) + e1.x0;
        scanLine(std::floor(x1), std::ceil(x0), y);
    }
}

// scan-line conversion
static void scanTriangle(const mbgl::vec2<double> a, const mbgl::vec2<double> b, const mbgl::vec2<double> c, int32_t ymin, int32_t ymax, ScanLine& scanLine) {
    edge ab = edge(a, b);
    edge bc = edge(b, c);
    edge ca = edge(c, a);

    // sort edges by y-length
    if (ab.dy > bc.dy) { std::swap(ab, bc); }
    if (ab.dy > ca.dy) { std::swap(ab, ca); }
    if (bc.dy > ca.dy) { std::swap(bc, ca); }

    // scan span! scan span!
    if (ab.dy) scanSpans(ca, ab, ymin, ymax, scanLine);
    if (bc.dy) scanSpans(ca, bc, ymin, ymax, scanLine);
}

std::forward_list<TileID> tileCover(int8_t z, const mbgl::box &bounds) {
    int32_t tiles = 1 << z;
    std::forward_list<mbgl::TileID> t;

    auto scanLine = [&](int32_t x0, int32_t x1, int32_t y) {
        int32_t x;
        if (y >= 0 && y <= tiles) {
            for (x = x0; x < x1; x++) {
                t.emplace_front(z, x, y);
            }
        }
    };

    // Divide the screen up in two triangles and scan each of them:
    // \---+
    // | \ |
    // +---\.
    scanTriangle(bounds.tl, bounds.tr, bounds.br, 0, tiles, scanLine);
    scanTriangle(bounds.br, bounds.bl, bounds.tl, 0, tiles, scanLine);

    t.unique();

    return t;
}

}
