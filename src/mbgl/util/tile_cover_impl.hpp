#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/util/geometry.hpp>

#include <vector>
#include <map>

namespace mbgl {

class TransformState;
class LatLngBounds;

namespace util {

struct bound;

using point_list = std::vector<Point<double>>;
using bound_list = std::vector<bound>;
using edge_table = std::map<uint32_t, bound_list>;
struct x_range {
    int32_t x0;
    int32_t x1;
    bool winding;
};
    
struct bound {
    point_list points;
    size_t currentPointIndex = 0;
    double currentX;
    bool winding;

    double interpolate(uint32_t y) {
        const auto& p0 = points[currentPointIndex];
        const auto& p1 = points[currentPointIndex + 1];

        const auto dx = p1.x - p0.x;
        const auto dy = p1.y - p0.y;
        currentX = p0.x;
        if (dx == 0) {
            return currentX;
        } else if (dy == 0){
            return y <= p0.y ? p0.x : p1.x;
        }
        if (y < p0.y) return currentX;
        if (y > p1.y) return p1.x;
        currentX = (dx / dy) * (y - p0.y) + p0.x;
        return currentX;
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
    bool isClosed;
    uint32_t currentRow;
    bound_list activeEdgeTable;
    edge_table edgeTable;
    uint32_t maxY;
};


} // namespace util
} // namespace mbgl
