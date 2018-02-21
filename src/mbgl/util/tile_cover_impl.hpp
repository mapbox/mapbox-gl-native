#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/math/clamp.hpp>
#include <vector>
#include <map>

namespace mbgl {

class TransformState;
class LatLngBounds;

namespace util {

struct edge;
struct bound;

using edge_list = std::vector<edge>;
using bound_list = std::vector<bound>;
using edge_table = std::map<uint32_t, bound_list>;
using x_range = std::pair<int32_t, int32_t>;
static const double INF = 1e20;

struct edge {
    Point<double> p0, p1;
    double x, m;
    edge(Point<double> _a, Point<double> _b) {
        if (_a.y > _b.y) {
            std::swap(_a, _b);
        }
        p0 = _a;
        p1 = _b;

        const auto dx = p1.x - p0.x;
        const auto dy = p1.y - p0.y;
        x = p0.x;
        if (dy == 0) {
            m = INF;
        }
        else {
            m = dx / dy;
        }
    }

    double interpolate(uint32_t y) {
        if (m == 0) { return x; }

        if ( y < p0.y || y > p1.y) {
            util::clamp(y, static_cast<uint32_t>(p0.y), static_cast<uint32_t>(p1.y));
        }

        x = m * (y - p0.y) + p0.x;
        return x;
    }
};

struct bound {
    edge_list edges;
    size_t currentEdgeIndex = 0;

    edge& currentEdge() {
        if(currentEdgeIndex >= edges.size()) { return edges.back(); };
        return edges[currentEdgeIndex];
    }

    const edge& current() const {
        if(currentEdgeIndex >= edges.size()) { return edges.back(); };
        return edges[currentEdgeIndex];
    }
};

using ScanLine = const std::function<void(int32_t x0, int32_t x1, int32_t y)>;

class TileCoverImpl {

public:
    TileCoverImpl(int32_t z, const Geometry<double>& geom, bool project = true);
    bool scanRow(ScanLine& );
    bool next();
    void reset();
private:
    bool closed_geom;
    uint32_t current_y; // current scanLine
    bound_list aet;
    edge_table et;
    uint32_t max_y;
};


} // namespace util
} // namespace mbgl
