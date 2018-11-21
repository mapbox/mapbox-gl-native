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

// Implements a modified scan-line algorithm to provide a streaming interface for
// tile cover on arbitrary shapes.
// A `BoundsMap` is genereted from the input geometry where each tuple indicates
// the set of Bounds that start at a y tile coordinate. Each bound represents
// a chain of edges from a local y-minima to a local y-maxima.
// For each row, the activeBounds list aggregates all bounds that enter into or
// begin in that row. This running list of bounds is scanned, capturing the
// x-coordinates spanned by edges in a bound until the bound exits the row (or
// ends). The result is a set of (possibly overlapping) min,max pairs of x coordinates
// (spans). In the simplest case a span represents the x-coordinates at which a
// single edge intersects the top and bottom of a tile row. Interior tiles of a
// polygon are captured by merging spans using the non-zero rule.
// The result of a scan using `nextRow()` is a list of spans (tileXSpans) of x-coordinates
// that includes edges and interiors of polygons.
// next() returns a tileID for each x-coordinate from (first, second] in each
// span in tileXSpans.
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
