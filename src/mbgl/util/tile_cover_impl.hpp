#pragma once

#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/optional.hpp>

#include <vector>
#include <map>
#include <queue>

namespace mbgl {

class TransformState;
class LatLngBounds;

namespace util {

struct Bound;

using Bounds = std::vector<Bound>;
using BoundsMap = std::map<uint32_t, Bounds>;

// A chain of points from a local minimum to a local maximum. `winding` indicates
//  the direction of the original geometry.
struct Bound {
    std::vector<Point<double>> points;
    size_t currentPoint = 0;
    bool winding = false;

    Bound() = default;
    Bound(const Bound& rhs) {
        points = rhs.points;
        currentPoint = rhs.currentPoint;
        winding = rhs.winding;
    }
    Bound& operator=(Bound&& rhs) {
        points = std::move(rhs.points);
        currentPoint = rhs.currentPoint;
        winding = rhs.winding;
        return *this;
    }

    // Compute the interpolated x coordinate at y for the current edge
    double interpolate(uint32_t y) {
        const auto& p0 = points[currentPoint];
        const auto& p1 = points[currentPoint + 1];

        const auto dx = p1.x - p0.x;
        const auto dy = p1.y - p0.y;
        auto x = p0.x;
        if (dx == 0) {
            return x;
        } else if (dy == 0){
            return y <= p0.y ? p0.x : p1.x;
        }
        if (y < p0.y) return x;
        if (y > p1.y) return p1.x;
        x = (dx / dy) * (y - p0.y) + p0.x;
        return x;
    }
};

class TileCover::Impl {
public:
    Impl(int32_t z, const Geometry<double>& geom, bool project = true);
    ~Impl() = default;

    optional<UnwrappedTileID> next();
    bool hasNext() const;

private:
    using TileSpans = std::queue<std::pair<int32_t, int32_t>>;

    void nextRow();

    const int32_t zoom;
    bool isClosed;

    BoundsMap boundsMap;
    BoundsMap::iterator currentBounds;
    // List of bounds that begin at or before `tileY`
    Bounds activeBounds;

    TileSpans tileXSpans;
    uint32_t tileY;
    int32_t tileX;
};

} // namespace util
} // namespace mbgl
