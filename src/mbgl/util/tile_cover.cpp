#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/tile_cover_impl.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/math/log2.hpp>

#include <functional>
#include <list>

namespace mbgl {

namespace {

using ScanLine = const std::function<void(int32_t x0, int32_t x1, int32_t y)>;

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

// scan-line conversion
static void scanSpans(edge e0, edge e1, int32_t ymin, int32_t ymax, ScanLine scanLine) {
    double y0 = ::fmax(ymin, std::floor(e1.y0));
    double y1 = ::fmin(ymax, std::ceil(e1.y1));

    // sort edges by x-coordinate
    double m0 = e0.dx / e0.dy;
    double m1 = e1.dx / e1.dy;
    double ySort = e0.y0 == e1.y0 ? std::min(e0.y1, e1.y1) : std::max(e0.y0, e1.y0);
    if (e0.x0 - (e0.y0 - ySort) * m0 < e1.x0 - (e1.y0 - ySort) * m1) {
        std::swap(e0, e1);
        std::swap(m0, m1);
    }

    // scan lines!
    double d0 = e0.dx > 0; // use y + 1 to compute x0
    double d1 = e1.dx < 0; // use y + 1 to compute x1
    for (int32_t y = y0; y < y1; y++) {
        double x0 = m0 * ::fmax(0, ::fmin(e0.dy, y + d0 - e0.y0)) + e0.x0;
        double x1 = m1 * ::fmax(0, ::fmin(e1.dy, y + d1 - e1.y0)) + e1.x0;
        scanLine(std::floor(x1), std::ceil(x0), y);
    }
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
    const int32_t tiles = (1 << z) + 1;

    struct ID {
        int32_t x, y;
        double sqDist;
    };

    std::vector<ID> t;

    auto scanLine = [&](int32_t x0, int32_t x1, int32_t y) {
        int32_t x;
        for (x = x0; x < x1; ++x) {
            const auto dx = x + 0.5 - c.x, dy = y + 0.5 - c.y;
            t.emplace_back(ID { x, y, dx * dx + dy * dy });
        }
    };

    std::vector<Point<double>> bounds = {tl, tr, br, bl};
    while (bounds[0].y > min(min(bounds[1].y, bounds[2].y), bounds[3].y)) {
        std::rotate(bounds.begin(), bounds.begin() + 1, bounds.end());
    }
    /*
    Keeping the clockwise winding order (abcd), we rotated convex quadrilateral
    angles in such way that angle a (bounds[0]) is on top):
              a
            /   \
           /     b
          /      |
         /       c
        /  ....
       / ..
      d
    This is an example: we handle also cases where d.y < c.y, d.y < b.y etc.
    Split the scan to tree steps:
              a
            /   \     (1)
           /     b
     -----------------
          /      |    (2)
         /       c
     -----------------
        /  ....
       / ..           (3)
      d
    */
    edge ab = edge(bounds[0], bounds[1]);
    edge ad = edge(bounds[0], bounds[3]);

    // Scan (1).
    int32_t ymin = std::floor(bounds[0].y);
    if (bounds[3].y < bounds[1].y) { std::swap(ab, ad); }
    int32_t ymax = std::ceil(ab.y1);
    if (ab.dy) {
        scanSpans(ad, ab, std::max(0, ymin), std::min(tiles, ymax), scanLine);
        ymin = ymax;
    }

    // Scan (2).
    // yCutLower is c or d, whichever is with lower y value.
    float yCutLower = min(bounds[2].y, ad.y1);
    ymax = std::ceil(yCutLower);

    // bc is edge opposite of ad.
    edge bc = bounds[3].y < bounds[1].y ? edge(bounds[3], bounds[2]) : edge(bounds[1], bounds[2]);
    if (bc.dy) {
        scanSpans(ad, bc, std::max(0, ymin), std::min(tiles, ymax), scanLine);
        ymin = ymax;
    } else {
        ymin = std::floor(yCutLower);
    }

    // Scan (3) - the triangle at the bottom.
    if (ad.y1 < bc.y1) { std::swap(ad, bc); }
    ymax = std::ceil(ad.y1);
    bc = edge({ bc.x1, bc.y1 }, { ad.x1, ad.y1 });
    if (bc.dy) { scanSpans(ad, bc, std::max(0, ymin), std::min(tiles, ymax), scanLine); }

    // Sort first by distance, then by x/y.
    std::sort(t.begin(), t.end(), [](const ID& a, const ID& b) {
        return std::tie(a.sqDist, a.x, a.y) < std::tie(b.sqDist, b.x, b.y);
    });

    assert(t.end() == std::unique(t.begin(), t.end(), [](const ID& a, const ID& b) {
        return a.x == b.x && a.y == b.y;
    })); // no duplicates.

    std::vector<UnwrappedTileID> result;
    for (const auto& id : t) {
        result.emplace_back(z, id.x, id.y);
    }
    return result;
}

} // namespace

int32_t coveringZoomLevel(double zoom, style::SourceType type, uint16_t size) {
    zoom += util::log2(util::tileSize / size);
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
        Projection::project(bounds.northwest(), z),
        Projection::project(bounds.northeast(), z),
        Projection::project(bounds.southeast(), z),
        Projection::project(bounds.southwest(), z),
        Projection::project(bounds.center(), z),
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

std::vector<UnwrappedTileID> tileCover(const Geometry<double>& geometry, int32_t z) {
    std::vector<UnwrappedTileID> result;
    TileCover tc(geometry, z, true);
    while (tc.hasNext()) {
        result.push_back(*tc.next());
    };

    return result;
}

// Taken from https://github.com/mapbox/sphericalmercator#xyzbbox-zoom-tms_style-srs
// Computes the projected tiles for the lower left and upper right points of the bounds
// and uses that to compute the tile cover count
uint64_t tileCount(const LatLngBounds& bounds, uint8_t zoom){
    if (zoom == 0) {
        return 1;
    }
    auto sw = Projection::project(bounds.southwest(), zoom);
    auto ne = Projection::project(bounds.northeast(), zoom);
    auto maxTile = std::pow(2.0, zoom);
    auto x1 = floor(sw.x);
    auto x2 = ceil(ne.x) - 1;
    auto y1 = util::clamp(floor(sw.y), 0.0, maxTile - 1);
    auto y2 = util::clamp(floor(ne.y), 0.0, maxTile - 1);

    auto dx = x1 > x2 ? (maxTile - x1) + x2 : x2 - x1;
    auto dy = y1 - y2;
    return (dx + 1) * (dy + 1);
}

uint64_t tileCount(const Geometry<double>& geometry, uint8_t z) {
    uint64_t tileCount = 0;

    TileCover tc(geometry, z, true);
    while (tc.next()) {
        tileCount++;
    };
    return tileCount;
}

TileCover::TileCover(const LatLngBounds&bounds_, int32_t z) {
    LatLngBounds bounds = LatLngBounds::hull(
        { std::max(bounds_.south(), -util::LATITUDE_MAX), bounds_.west() },
        { std::min(bounds_.north(),  util::LATITUDE_MAX), bounds_.east() });

    if (bounds.isEmpty() ||
        bounds.south() >  util::LATITUDE_MAX ||
        bounds.north() < -util::LATITUDE_MAX) {
        bounds = LatLngBounds::world();
    }

    auto sw = Projection::project(bounds.southwest(), z);
    auto ne = Projection::project(bounds.northeast(), z);
    auto se = Projection::project(bounds.southeast(), z);
    auto nw = Projection::project(bounds.northwest(), z);

    Polygon<double> p({ {sw, nw, ne, se, sw} });
    impl = std::make_unique<TileCover::Impl>(z, p, false);
}

TileCover::TileCover(const Geometry<double>& geom, int32_t z, bool project/* = true*/)
 : impl( std::make_unique<TileCover::Impl>(z, geom, project)) {
}

TileCover::~TileCover() = default;

optional<UnwrappedTileID> TileCover::next() {
    return impl->next();
}

bool TileCover::hasNext() {
    return impl->hasNext();
}

} // namespace util
} // namespace mbgl
