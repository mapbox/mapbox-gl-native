#include <mbgl/util/tile_cover_impl.hpp>

#include <functional>
#include <cmath>
#include <assert.h>
#include <limits.h>
#include <algorithm>

namespace mbgl {
namespace util {

void start_list_on_local_minimum(point_list& points) {
    assert(points.size() > 2);
    // Find the first local minimum going forward in the list
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
    //Re-close the linear ring
    points.erase(std::prev(points.end()));
    std::rotate(points.begin(), pt, points.end());
    points.push_back(*points.begin());
}

bound create_bound_towards_maximum(point_list& points, point_list::iterator& pt) {
    if (std::distance(pt, points.end()) < 2) { return {}; };
    if (std::distance(pt, points.end()) == 2) {
        bound bnd;
        if (points[0].y < points[1].y) std::copy(pt, points.end(), std::back_inserter(bnd.points));
        else std::reverse_copy(pt, points.end(), std::back_inserter(bnd.points));
        pt = points.end();
        return bnd;
    }
    const auto begin = pt;
    auto prev_pt = pt == points.begin() ? std::prev(points.end(), 2) : std::prev(pt);
    auto next_pt = std::next(pt) == points.end() ? std::next(points.begin()) : std::next(pt);
    while (pt != points.end()) {
        if ((pt->y >= prev_pt->y) &&
            (pt->y > next_pt->y )) {
            break;
        }
        prev_pt = pt;
        pt++;
        next_pt++;
        if (next_pt == points.end()) { next_pt = std::next(points.begin()); }
    }

    bound bnd;
    if (std::next(pt) == points.end()) { next_pt = points.end(); pt++; };
    bnd.points.reserve(static_cast<std::size_t>(std::distance(begin, next_pt)));
    std::copy(begin, next_pt, std::back_inserter(bnd.points));

    return bnd;
}

bound create_bound_towards_minimum(point_list& points, point_list::iterator& pt) {
    if (std::distance(pt, points.end()) < 2) { return {}; };
    if (std::distance(pt, points.end()) == 2) {
        bound bnd;
        if (pt->y < std::next(pt)->y) std::copy(pt, points.end(), std::back_inserter(bnd.points));
        else std::reverse_copy(pt, points.end(), std::back_inserter(bnd.points));
        pt = points.end();
        return bnd;
    }
    auto begin = pt;
    auto prev_pt = pt == points.begin() ? std::prev(points.end(), 2) : std::prev(pt);
    auto next_pt = std::next(pt) == points.end() ? std::next(points.begin()) : std::next(pt);
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

    bound bnd;
    if (std::next(pt) == points.end()) { next_pt = points.end(); pt++; };
    bnd.points.reserve(static_cast<std::size_t>(std::distance(begin, next_pt)));
    std::reverse_copy(begin, next_pt, std::back_inserter(bnd.points));
    return bnd;
}

void build_edge_table(point_list& points, uint32_t maxTile, edge_table& et, bool closed = false) {
    if (closed) {
        start_list_on_local_minimum(points);
    }
    auto pointsIter = points.begin();
    while (pointsIter != points.end()) {
        bound to_max = create_bound_towards_maximum(points, pointsIter);
        bound to_min = create_bound_towards_minimum(points, pointsIter);

        if (to_max.points.size() > 0) {
            // Projections may result in values beyond the bounds due to double precision
            const auto y = static_cast<uint32_t>(std::floor(clamp(to_max.points.front().y, 0.0, (double)maxTile)));
            to_max.interpolate(y);
            et[y].push_back(std::move(to_max));
        }
        if (to_min.points.size() > 0) {
            const auto y = static_cast<uint32_t>(std::floor(clamp(to_min.points.front().y, 0.0, (double)maxTile)));
            to_min.interpolate(y);
            et[y].push_back(std::move(to_min));
        }
    }
    assert(pointsIter == points.end());
}

std::vector<x_range> scan_row(uint32_t y, bound_list& aet) {
    std::vector<x_range> tile_range;
    tile_range.reserve(aet.size());

    for(bound& b: aet) {
        x_range xp = { INT_MAX, 0 };
        double x;
        const auto numEdges = b.points.size() - 1;
        while (b.currentPointIndex < numEdges) {
            x = b.currentX;
            xp.first = std::min(xp.first, static_cast<int32_t>(std::floor(x)));
            xp.second = std::max(xp.second, static_cast<int32_t>(std::ceil(x)));

            // If this edge ends beyond the current row, find the x-value at the exit,
            //  and be done with this bound
            auto& p1 = b.points[b.currentPointIndex + 1];
            if (p1.y > y+1) {
                x = b.interpolate(y+1);
                xp.first = std::min(xp.first, static_cast<int32_t>(std::floor(x)));
                xp.second = std::max(xp.second, static_cast<int32_t>(std::ceil(x)));
                break;
            } else if(b.currentPointIndex == numEdges - 1) {
                // For last edge, consider x at edge end;
                x = p1.x;
                xp.first = std::min(xp.first, static_cast<int32_t>(std::floor(x)));
                xp.second = std::max(xp.second, static_cast<int32_t>(std::ceil(x)));
            }
            b.currentPointIndex++;
        }
        tile_range.push_back(xp);
    }
    // Erase bounds in the aet whose current edge ends inside this row, or there are no more edges
    auto bound = aet.begin();
    while (bound != aet.end()) {
        if ( bound->currentPointIndex == bound->points.size() - 1 &&
            bound->points[bound->currentPointIndex].y <= y+1) {
            bound = aet.erase(bound);
        } else {
            bound++;
        }
    }
    // Sort the X-extents of each lml by x_min, x_max
    std::sort(tile_range.begin(), tile_range.end(), [] (x_range& a, x_range& b) {
        return std::tie(a.first, a.second) < std::tie(b.first, b.second);
    });

    return tile_range;
}

struct ToEdgeTables {
    int32_t zoom;
    bool project = false;
    ToEdgeTables(int32_t z, bool p): zoom(z), project(p) {}

    void buildTable(const std::vector<Point<double>>& points, edge_table& et, bool closed = false) const {
        point_list projectedPoints;
        if (project) {
            projectedPoints.reserve(points.size());
            for(const auto&p : points) {
                projectedPoints.push_back(
                    Projection::project(LatLng{ p.y, p.x }, zoom));
            }
        } else {
            projectedPoints.insert(projectedPoints.end(), points.begin(), points.end());
        }
        build_edge_table(projectedPoints, 1 << zoom, et, closed);
    }
    
    edge_table buildPolygonTable(const Polygon<double>& polygon) const {
        edge_table et;
        for(const auto&ring : polygon) {
            buildTable(ring, et, true);
        }
        return et;
    }
    std::vector<edge_table> operator()(const Point<double>&) const {
        return { };
    }

    std::vector<edge_table> operator()(const MultiPoint<double>&) const {
        return { };
    }

    std::vector<edge_table> operator()(const LineString<double>& lines) const {
        edge_table et;
        buildTable(lines, et);
        return { et };
    }

    std::vector<edge_table> operator()(const MultiLineString<double>& lines) const {
        edge_table et;
        for(const auto&line : lines) {
            buildTable(line, et);
        }
        return { et };
    }

    std::vector<edge_table> operator()(const Polygon<double>& polygon) const {
        return { buildPolygonTable(polygon) };
    }

    std::vector<edge_table> operator()(const MultiPolygon<double>& polygons) const {
        std::vector<edge_table> tables;
        for(const auto& polygon: polygons) {
            tables.push_back(buildPolygonTable(polygon));
        }
        return tables;
    }

    std::vector<edge_table> operator()(const mapbox::geometry::geometry_collection<double>&) const {
        return {};
    }
};

TileCoverImpl::TileCoverImpl(int32_t z, const Geometry<double>& geom, bool project): maxY(1 << z) {
    ToFeatureType toFeatureType;
    isClosed = apply_visitor(toFeatureType, geom) == FeatureType::Polygon;

    ToEdgeTables toEdgeTables(z, project);
    edgeTables = apply_visitor(toEdgeTables, geom);
    reset();
}

void TileCoverImpl::reset() {
    currentEdgeTable = edgeTables.begin();
    if (currentEdgeTable != edgeTables.end()) {
        activeEdgeTable = currentEdgeTable->begin()->second;
        currentY = currentEdgeTable->begin()->first;
    }
}

bool TileCoverImpl::next() {
    return currentEdgeTable != edgeTables.end() && activeEdgeTable.size() != 0 && currentY < maxY;
}

bool TileCoverImpl::scanRow(ScanLine& scanCover) {
    if (!next()) { return false; }

    auto xps = util::scan_row(currentY, activeEdgeTable);
    auto p = xps.begin();
    assert(isClosed ? xps.size() % 2 == 0 : true);
    while (p != xps.end()) {
        auto x_min = p->first;
        // For closed geometries, consider odd even pairs of tile ranges
        if (isClosed) { p++; }
        auto x_max = p++->second;
        scanCover(x_min, x_max, currentY);
    }
    // Move to the next row
    currentY++;

    // Update AET for next row
    auto nextRow = currentEdgeTable->find(currentY);
    if (nextRow != currentEdgeTable->end()) {
        activeEdgeTable.insert(activeEdgeTable.end(), nextRow->second.begin(), nextRow->second.end());
    } else if (activeEdgeTable.size() == 0){
        currentEdgeTable++;
        if (currentEdgeTable == edgeTables.end()) return false;
        activeEdgeTable = currentEdgeTable->begin()->second;
        currentY = currentEdgeTable->begin()->first;
    }
    return next();
}

} // namespace util
} // namespace mbgl
