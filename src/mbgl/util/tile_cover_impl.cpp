#include <mbgl/util/tile_cover_impl.hpp>
#include <mbgl/util/tile_coordinate.hpp>

#include <functional>
#include <cmath>
#include <assert.h>
#include <limits.h>
#include <algorithm>

namespace mbgl {
namespace util {

using PointList = std::vector<Point<double>>;

struct TileSpan {
    int32_t xmin, xmax;
    bool winding;
};

// Reorder a ring of points such that it starts at a point with a local minimum y-coordinate
void start_list_on_local_minimum(PointList& points) {
    auto prev_pt = std::prev(points.end(), 2);
    auto pt = points.begin();
    auto next_pt = std::next(pt);
    while (pt != points.end()) {
        if ((pt->y <= prev_pt->y) &&
            (pt->y < next_pt->y)) {
            break;
        }
        prev_pt = pt;
        pt++;
        next_pt++;
        if (next_pt == points.end()) { next_pt = std::next(points.begin()); }
    }
    if (pt == points.end())
        return;
    //Re-close linear rings with first_pt = last_pt
    if (points.back() == points.front()) {
        points.pop_back();
    }
    std::rotate(points.begin(), pt, points.end());
    points.push_back(*points.begin());
}

//Create a bound towards a local maximum point, starting from pt.
// Traverse from current pt until the next pt changes y-direction, and copy
// all points from start to end (inclusive) into a Bound.
Bound create_bound_towards_maximum(PointList& points, PointList::iterator& pt) {
    if (std::distance(pt, points.end()) < 2) { return {}; }

    const auto begin = pt;
    auto next_pt = std::next(begin);
    while (pt->y <= next_pt->y) {
        pt++;
        next_pt++;
        if (next_pt == points.end()) { pt++; break; }
    }

    const auto pt_distance = std::distance(begin, next_pt);
    if (pt_distance < 2) {
        return {};
    }

    Bound bnd;
    bnd.points.reserve(static_cast<std::size_t>(std::distance(begin, next_pt)));
    std::copy(begin, next_pt, std::back_inserter(bnd.points));
    bnd.winding = true;
    return bnd;
}

//Create a bound towards a local minimum point, starting from pt.
// Traverse from current pt until the next pt changes y-direction, and copy
// all points from start to end (inclusive) into a Bound.
Bound create_bound_towards_minimum(PointList& points, PointList::iterator& pt) {
    if (std::distance(pt, points.end()) < 2) { return {}; }

    auto begin = pt;
    auto next_pt = std::next(begin);
    while (pt->y > next_pt->y) {
        pt++;
        next_pt++;
        if (next_pt == points.end()) { pt++; break; }
    }

    const auto pt_distance = std::distance(begin, next_pt);
    if (pt_distance < 2) {
        return {};
    }
    Bound bnd;
    bnd.points.reserve(static_cast<std::size_t>(std::distance(begin, next_pt)));
    //For bounds that start at a max, reverse copy so that all bounds start at a min
    std::reverse_copy(begin, next_pt, std::back_inserter(bnd.points));
    bnd.winding = false;
    return bnd;
}

// Given a set of points (ring or list) representing a shape, compute a set of
// Bounds, where each Bound represents edges going from a local minima to a local
// maxima point. The BoundsMap is an edge table indexed on the starting Y-tile
// of each Bound.
void build_bounds_map(PointList& points, uint32_t maxTile, BoundsMap& et, bool closed = false) {
    if (points.size() < 2) return;
    //While traversing closed rings, start the bounds at a local minimum.
    // (For linestrings the starting point is always a local maxima/minima)
    if (closed) {
        start_list_on_local_minimum(points);
    }

    auto pointsIter = points.begin();
    while (pointsIter != points.end()) {
        Bound to_max = create_bound_towards_maximum(points, pointsIter);
        Bound to_min = create_bound_towards_minimum(points, pointsIter);

        if (to_max.points.size() >= 2) {
            // Projections may result in values beyond the bounds, clamp to max tile coordinates
            const auto y = static_cast<uint32_t>(std::floor(clamp(to_max.points.front().y, 0.0, (double)maxTile)));
            et[y].push_back(to_max);
        }
        if (to_min.points.size() >= 2) {
            const auto y = static_cast<uint32_t>(std::floor(clamp(to_min.points.front().y, 0.0, (double)maxTile)));
            et[y].push_back(to_min);
        }
    }
    assert(pointsIter == points.end());
}

void update_span(TileSpan& xp, double x) {
    xp.xmin = std::min(xp.xmin, static_cast<int32_t>(std::floor(x)));
    xp.xmax = std::max(xp.xmax, static_cast<int32_t>(std::ceil(x)));
}

// Use the active bounds, an accumulation of all bounds that enter the y tile row,
// or start in that row.
// Iterate all points of a bound until it exits the row (or ends) and compute the
// set of X tiles it spans across. The winding direction of the bound is also
// captured for each span to later fill tiles between bounds for polygons
std::vector<TileSpan> scan_row(uint32_t y, Bounds& activeBounds) {
    std::vector<TileSpan> tile_range;
    tile_range.reserve(activeBounds.size());

    for(Bound& b: activeBounds) {
        TileSpan xp = { INT_MAX, 0, b.winding };
        double x;
        const auto numEdges = b.points.size() - 1;
        while (b.currentPoint < numEdges) {
            x = b.interpolate(y);
            update_span(xp, x);

            // If this edge ends beyond the current row, find the x-intercept where
            // it exits the row
            auto& p1 = b.points[b.currentPoint + 1];
            if (p1.y > y+1) {
                x = b.interpolate(y+1);
                update_span(xp, x);
                break;
            } else if (b.currentPoint == numEdges - 1) {
                // For last edge, consider x-intercept at the end of the edge.
                x = p1.x;
                update_span(xp, x);
            }
            b.currentPoint++;
        }
        tile_range.push_back(xp);
    }
    // Erase bounds in the active table whose current edge ends inside this row,
    // or there are no more edges
    auto bound = activeBounds.begin();
    while (bound != activeBounds.end()) {
        if ( bound->currentPoint == bound->points.size() - 1 &&
            bound->points[bound->currentPoint].y <= y+1) {
            bound = activeBounds.erase(bound);
        } else {
            bound++;
        }
    }
    // Sort the X-extents of each crossing bound by x_min, x_max
    std::sort(tile_range.begin(), tile_range.end(), [] (TileSpan& a, TileSpan& b) {
        return std::tie(a.xmin, a.xmax) < std::tie(b.xmin, b.xmax);
    });

    return tile_range;
}

struct BuildBoundsMap {
    int32_t zoom;
    bool project = false;
    BuildBoundsMap(int32_t z, bool p): zoom(z), project(p) {}

    void buildTable(const std::vector<Point<double>>& points, BoundsMap& et, bool closed = false) const {
        PointList projectedPoints;
        if (project) {
            projectedPoints.reserve(points.size());
            for(const auto&p : points) {
                projectedPoints.push_back(
                    Projection::project(LatLng{ p.y, p.x }, zoom));
            }
        } else {
            projectedPoints.insert(projectedPoints.end(), points.begin(), points.end());
        }
        build_bounds_map(projectedPoints, 1 << zoom, et, closed);
    }

    void buildPolygonTable(const Polygon<double>& polygon, BoundsMap& et) const {
        for(const auto&ring : polygon) {
            buildTable(ring, et, true);
        }
    }
    BoundsMap operator()(const Point<double>&p) const {
        Bound bnd;
        auto point = p;
        if (project) {
            point = Projection::project(LatLng{p.y, p.x}, zoom);
        }
        bnd.points.insert(bnd.points.end(), 2, point);
        bnd.winding = false;
        BoundsMap et;
        const auto y = static_cast<uint32_t>(std::floor(clamp(point.y, 0.0, (double)(1 << zoom))));
        et[y].push_back(bnd);
        return et;
    }

    BoundsMap operator()(const MultiPoint<double>& points) const {
        BoundsMap et;
        for (const Point<double>& p: points) {
            Bound bnd;
            auto point = p;
            if (project) {
                point = Projection::project(LatLng{p.y, p.x}, zoom);
            }
            bnd.points.insert(bnd.points.end(), 2, point);
            bnd.winding = false;
            const auto y = static_cast<uint32_t>(std::floor(clamp(point.y, 0.0, (double)(1 << zoom))));
            et[y].push_back(bnd);
        }
        return et;
    }

    BoundsMap operator()(const LineString<double>& lines) const {
        BoundsMap et;
        buildTable(lines, et);
        return et;
    }

    BoundsMap operator()(const MultiLineString<double>& lines) const {
        BoundsMap et;
        for(const auto&line : lines) {
            buildTable(line, et);
        }
        return et;
    }

    BoundsMap operator()(const Polygon<double>& polygon) const {
        BoundsMap et;
        buildPolygonTable(polygon, et);
        return et;
    }

    BoundsMap operator()(const MultiPolygon<double>& polygons) const {
        BoundsMap et;
        for(const auto& polygon: polygons) {
            buildPolygonTable(polygon, et);
        }
        return et;
    }

    BoundsMap operator()(const mapbox::geometry::geometry_collection<double>&) const {
        return {};
    }
};

TileCover::Impl::Impl(int32_t z, const Geometry<double>& geom, bool project)
 : zoom(z) {
    ToFeatureType toFeatureType;
    isClosed = apply_visitor(toFeatureType, geom) == FeatureType::Polygon;

    BuildBoundsMap toBoundsMap(z, project);
    boundsMap = apply_visitor(toBoundsMap, geom);
    if (boundsMap.size() == 0) return;

    //Iniitalize the active edge table, and current row span
    currentBounds = boundsMap.begin();
    tileY = 0;
    nextRow();
    if (tileXSpans.empty()) return;
    tileX = tileXSpans.front().first;
}

// Aggregate all Bounds that start in or enter into the next tileY row. Multi-geoms
// may have discontinuity in the BoundMap, so skip forward to the next tileY row
// when the current/next row has no more bounds in it.
// Use scan_row to generate the tileX spans. Merge spans to avoid duplicate tiles
// in TileCoverImpl::next(). For closed geometry, use the non-zero rule to expand
// (fill) tiles between pairs of spans.
void TileCover::Impl::nextRow() {
    // Update activeBounds for next row
    if (currentBounds != boundsMap.end()) {
        if (activeBounds.size() == 0 && currentBounds->first > tileY) {
            //For multi-geoms: use the next row with an edge table starting point
            tileY = currentBounds->first;
        }
        if (tileY == currentBounds->first) {
            std::move(currentBounds->second.begin(), currentBounds->second.end(),
                std::back_inserter(activeBounds));
            currentBounds++;
        }
    }
    //Scan the active bounds and update currentRange with x_min, x_max pairs
    auto xps = util::scan_row(tileY, activeBounds);
    if (xps.size() == 0) {
        return;
    }

    auto x_min = xps[0].xmin;
    auto x_max = xps[0].xmax;
    int32_t nzRule = xps[0].winding ? 1 : -1;
    for (size_t i = 1; i < xps.size(); i++) {
        auto xp = xps[i];
        if (!(isClosed && nzRule != 0)) {
            if (xp.xmin > x_max && xp.xmax >= x_max) {
                tileXSpans.emplace(x_min, x_max);
                x_min = xp.xmin;
            }
        }
        nzRule += xp.winding ? 1 : -1;
        x_max = std::max(x_min, xp.xmax);
    }
    tileXSpans.emplace(x_min, x_max);
}

bool TileCover::Impl::hasNext() const {
    return (!tileXSpans.empty()
            && tileX < tileXSpans.front().second
            && tileY < (1u << zoom));
}

optional<UnwrappedTileID> TileCover::Impl::next() {
    if (!hasNext()) return {};

    const auto x = tileX;
    const auto y = tileY;
    tileX++;
    if (tileX >= tileXSpans.front().second) {
        tileXSpans.pop();
        if (tileXSpans.empty()) {
            tileY++;
            nextRow();
        }
        if (!tileXSpans.empty()) {
            tileX = tileXSpans.front().first;
        }
    }
    return UnwrappedTileID(zoom, x, y);
}

} // namespace util
} // namespace mbgl
