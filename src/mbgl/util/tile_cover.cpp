#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/map/transform_state.hpp>

#include <functional>

namespace mbgl {

namespace {

// Taken from polymaps src/Layer.js
// https://github.com/simplegeo/polymaps/blob/master/src/Layer.js#L333-L383
struct edge {
    double x0 = 0, y0 = 0;
    double x1 = 0, y1 = 0;
    double dx = 0, dy = 0;

    edge(Point<double> a, Point<double> b) {
        if (a.y > b.y) std::swap(a, b);
        x0 = a.x;
        y0 = a.y;
        x1 = b.x;
        y1 = b.y;
        dx = b.x - a.x;
        dy = b.y - a.y;
    }
};

using ScanLine = const std::function<void(int32_t x0, int32_t x1, int32_t y)>;

// scan-line conversion
static void scanSpans(edge e0, edge e1, int32_t ymin, int32_t ymax, ScanLine scanLine) {
    double y0 = ::fmax(ymin, std::floor(e1.y0));
    double y1 = ::fmin(ymax, std::ceil(e1.y1));

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
        double x0 = m0 * ::fmax(0, ::fmin(e0.dy, y + d0 - e0.y0)) + e0.x0;
        double x1 = m1 * ::fmax(0, ::fmin(e1.dy, y + d1 - e1.y0)) + e1.x0;
        scanLine(std::floor(x1), std::ceil(x0), y);
    }
}

// scan-line conversion
static void scanTriangle(const Point<double>& a, const Point<double>& b, const Point<double>& c, int32_t ymin, int32_t ymax, ScanLine& scanLine) {
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

} // namespace

namespace util {

namespace {

std::vector<UnwrappedTileID> tileCover(const Point<double>& tl,
                                       const Point<double>& tr,
                                       const Point<double>& br,
                                       const Point<double>& bl,
                                       const Point<double>& c,
                                       int32_t z) {
    const int32_t tiles = 1 << z;

    struct ID {
        int32_t x, y;
        double sqDist;
    };

    std::vector<ID> t;

    auto scanLine = [&](int32_t x0, int32_t x1, int32_t y) {
        int32_t x;
        if (y >= 0 && y <= tiles) {
            for (x = x0; x < x1; ++x) {
                const auto dx = x + 0.5 - c.x, dy = y + 0.5 - c.y;
                t.emplace_back(ID{ x, y, dx * dx + dy * dy });
            }
        }
    };

    // Divide the screen up in two triangles and scan each of them:
    // \---+
    // | \ |
    // +---\.
    scanTriangle(tl, tr, br, 0, tiles, scanLine);
    scanTriangle(br, bl, tl, 0, tiles, scanLine);

    // Sort first by distance, then by x/y.
    std::sort(t.begin(), t.end(), [](const ID& a, const ID& b) {
        return std::tie(a.sqDist, a.x, a.y) < std::tie(b.sqDist, b.x, b.y);
    });

    // Erase duplicate tile IDs (they typically occur at the common side of both triangles).
    t.erase(std::unique(t.begin(), t.end(), [](const ID& a, const ID& b) {
                return a.x == b.x && a.y == b.y;
            }), t.end());

    std::vector<UnwrappedTileID> result;
    for (const auto& id : t) {
        result.emplace_back(z, id.x, id.y);
    }
    return result;
}

} // namespace

int32_t coveringZoomLevel(double zoom, style::SourceType type, uint16_t size) {
    zoom += std::log(util::tileSize / size) / std::log(2);
    if (type == style::SourceType::Raster || type == style::SourceType::Video) {
        return ::round(zoom);
    } else {
        return std::floor(zoom);
    }
}

std::vector<UnwrappedTileID> tileCover(const LatLngBounds& bounds_, int32_t z) {
    if (bounds_.isEmpty() ||
        bounds_.south() >  util::LATITUDE_MAX ||
        bounds_.north() < -util::LATITUDE_MAX) {
        return {};
    }

    LatLngBounds bounds = LatLngBounds::hull(
        { std::max(bounds_.south(), -util::LATITUDE_MAX), bounds_.west() },
        { std::min(bounds_.north(),  util::LATITUDE_MAX), bounds_.east() });

    return tileCover(
        TileCoordinate::fromLatLng(z, bounds.northwest()).p,
        TileCoordinate::fromLatLng(z, bounds.northeast()).p,
        TileCoordinate::fromLatLng(z, bounds.southeast()).p,
        TileCoordinate::fromLatLng(z, bounds.southwest()).p,
        TileCoordinate::fromLatLng(z, bounds.center()).p,
        z);
}

std::vector<UnwrappedTileID> tileCover(const TransformState& state, int32_t z) {
    assert(state.valid());

    const double w = state.getSize().width;
    const double h = state.getSize().height;
    return tileCover(
        TileCoordinate::fromScreenCoordinate(state, z, { 0,   0   }).p,
        TileCoordinate::fromScreenCoordinate(state, z, { w,   0   }).p,
        TileCoordinate::fromScreenCoordinate(state, z, { w,   h   }).p,
        TileCoordinate::fromScreenCoordinate(state, z, { 0,   h   }).p,
        TileCoordinate::fromScreenCoordinate(state, z, { w/2, h/2 }).p,
        z);
}

// Taken from https://github.com/mapbox/sphericalmercator#xyzbbox-zoom-tms_style-srs
// Computes the projected tiles for the lower left and upper right points of the bounds
// and uses that to compute the tile cover count
uint64_t tileCount(const LatLngBounds& bounds, uint8_t zoom, uint16_t tileSize_){

    auto sw = Projection::project(bounds.southwest().wrapped(), zoom, tileSize_);
    auto ne = Projection::project(bounds.northeast().wrapped(), zoom, tileSize_);

    auto x1 = floor(sw.x/ tileSize_);
    auto x2 = floor((ne.x - 1) / tileSize_);
    auto y1 = floor(sw.y/ tileSize_);
    auto y2 = floor((ne.y - 1) / tileSize_);

    auto minX = ::fmax(std::min(x1, x2), 0);
    auto maxX = std::max(x1, x2);
    auto minY = (std::pow(2, zoom) - 1) - std::max(y1, y2);
    auto maxY = (std::pow(2, zoom) - 1) - ::fmax(std::min(y1, y2), 0);
    
    return (maxX - minX + 1) * (maxY - minY + 1);
}

} // namespace util
} // namespace mbgl
