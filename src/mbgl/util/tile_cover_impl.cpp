#include <mbgl/util/tile_cover_impl.hpp>

#include <iostream>
#include <functional>
#include <cmath>
#include <assert.h>
#include <limits.h>
#include <algorithm>

namespace mbgl {
namespace util {

//namespace {
void start_list_on_local_minimum(edge_list& edges) {
    if (edges.size() <= 2) {
        return;
    }
    // Find the first local minimum going forward in the list
    auto prev_edge = edges.end();
    --prev_edge;
    auto edge = edges.begin();

    while (edge != edges.end()) {
        if (edge->p0 == prev_edge->p0) {
            break;
        }
        prev_edge = edge;
        ++edge;
    }
    std::rotate(edges.begin(), edge, edges.end());
}

bound create_bound_towards_maximum(edge_list& edges) {
    if (edges.size() == 0) return {};
    if (edges.size() == 1) {
        bound bnd;
        std::swap(bnd.edges, edges);
        return bnd;
    }
    auto next_edge = edges.begin();
    auto edge = next_edge;
    ++next_edge;

    while (next_edge != edges.end()) {
        if (edge->p1 == next_edge->p1) {
            break;
        }
        edge = next_edge;
        ++next_edge;
    }

    bound bnd;
    bnd.edges.reserve(static_cast<std::size_t>(std::distance(edges.begin(), next_edge)));
    std::move(edges.begin(), next_edge, std::back_inserter(bnd.edges));
    edges.erase(edges.begin(), next_edge);

    return bnd;
}

bound create_bound_towards_minimum(edge_list& edges) {
    if (edges.size() == 0) return {};
    if (edges.size() == 1) {
        bound bnd;
        std::swap(bnd.edges, edges);
        return bnd;
    }
    auto next_edge = edges.begin();
    auto edge = next_edge;
    ++next_edge;

    while (next_edge != edges.end()) {
        if (edge->p0 == next_edge->p0) {
            break;
        }
        edge = next_edge;
        ++next_edge;
    }

    bound bnd;

    bnd.edges.reserve(static_cast<std::size_t>(std::distance(edges.begin(), next_edge)));
    std::move(edges.begin(), next_edge, std::back_inserter(bnd.edges));
    edges.erase(edges.begin(), next_edge);
    std::reverse(bnd.edges.begin(), bnd.edges.end());

    return bnd;
}

void build_edge_table(edge_list& edges, uint32_t maxTile, edge_table& et, bool closed = false) {
    if (closed) {
        start_list_on_local_minimum(edges);
    }
    bound to_max, to_min;
    while (!edges.empty()) {
        to_max = create_bound_towards_maximum(edges);
        to_min = create_bound_towards_minimum(edges);

        if (to_max.edges.size() > 0) {
            // Projections may result in values beyond the bounds due to double precision
            const auto y = static_cast<uint32_t>(std::floor(util::clamp(to_max.edges.front().p0.y, 0.0, (double)maxTile)));
            et[y].push_back(std::move(to_max));
        }
        if (to_min.edges.size() > 0) {
            const auto y = static_cast<uint32_t>(std::floor(util::clamp(to_min.edges.front().p0.y, 0.0, (double)maxTile)));
            et[y].push_back(std::move(to_min));
        }
    }
    assert(edges.size() == 0);
}

std::vector<x_range> scan_row(uint32_t y, bound_list& aet) {
    std::vector<x_range> tile_range;
    tile_range.reserve(aet.size());

    for(bound& b: aet) {
        x_range xp = { INT_MAX, 0 };
        double x;
        const auto numEdges = b.edges.size();
        while (b.currentEdgeIndex < numEdges) {
            auto& e = b.currentEdge();
            x = e.x;
            xp.first = std::min(xp.first, static_cast<int32_t>(std::floor(x)));
            xp.second = std::max(xp.second, static_cast<int32_t>(std::ceil(x)));

            // If this edge ends beyond the current row, find the x-value at the exit,
            //  and be done with this bound
            if (e.p1.y > y+1) {
                x = e.interpolate(y+1);
                xp.first = std::min(xp.first, static_cast<int32_t>(std::floor(x)));
                xp.second = std::max(xp.second, static_cast<int32_t>(std::ceil(x)));
                break;
            } else if(b.currentEdgeIndex == numEdges - 1) {
                // For last edge, consider x at edge end;
                x = e.p1.x;
                xp.first = std::min(xp.first, static_cast<int32_t>(std::floor(x)));
                xp.second = std::max(xp.second, static_cast<int32_t>(std::ceil(x)));
            }
            b.currentEdgeIndex++;
        }
        tile_range.push_back(xp);
    }
    // Erase bounds in the aet whose current edge ends inside this row, or there are no more edges
    auto b = aet.begin();
    while (b != aet.end()) {
        if ( b->currentEdgeIndex >= b->edges.size() ||
            b->currentEdge().p1.y <= y+1) {
            b = aet.erase(b);
        } else {
            b++;
        }
    }
    // Sort the X-extents of each lml by x_min, x_max
    std::sort(tile_range.begin(), tile_range.end(), [] (x_range& a, x_range& b) {
        return std::tie(a.first, a.second) < std::tie(b.first, b.second);
    });

    return tile_range;
}

struct ToEdges {
    int32_t zoom;
    bool project;
    ToEdges(int32_t z, bool p): zoom(z), project(p) {}

    void makeEdges(const std::vector<Point<double>>& points, edge_list& edges) const {
        edges.reserve(edges.size() + points.size()-1);
        auto p1 = !project ? points[0] : Projection::project({points[0].y, points[0].x}, zoom);
        for (uint32_t j=1; j < points.size(); j++) {
            auto p2 = !project ? points[j] : Projection::project({points[j].y, points[j].x}, zoom);
            edges.emplace_back(p1, p2);
            p1 = p2;
        }
    }

    edge_list polyEdges(const Polygon<double>& g) const {
        edge_list edges;

        for (uint32_t i = 0; i < g.size(); i++) {
            const auto ring = g[i];
            makeEdges(ring, edges);
        }
        return edges;
    }

    std::vector<edge_list> operator()(const Point<double>& p) const {
        const auto pt = Projection::project({p.y, p.x}, zoom);
        return { { {pt, pt} } };
    }

    std::vector<edge_list> operator()(const MultiPoint<double>&points) const {
        std::vector<edge_list> edgeLists;
        edgeLists.reserve(points.size());
        for (const auto& p: points) {
            const auto pt = Projection::project({p.y, p.x}, zoom);
            edgeLists.push_back({ edge{pt, pt} });
        }
        return edgeLists;
    }

    std::vector<edge_list> operator()(const LineString<double>& lines) const {
        edge_list edges;
        makeEdges(lines, edges);
        return { edges };
    }

    std::vector<edge_list> operator()(const MultiLineString<double>& g) const {
        std::vector<edge_list> edgeLists;
        for(const auto& lines: g) {
            edge_list edges;
            makeEdges(lines, edges);
            edgeLists.push_back(edges);
            edges.clear();
        }
        return edgeLists;
    }

    std::vector<edge_list> operator()(const Polygon<double>& g) const {
        return { polyEdges(g) };
    }

    std::vector<edge_list> operator()(const MultiPolygon<double>& g) const {
        std::vector<edge_list> edgeLists;
        for(const auto& polygon: g) {
            edgeLists.push_back(polyEdges(polygon));
        }
        return edgeLists;
    }

    std::vector<edge_list> operator()(const mapbox::geometry::geometry_collection<double>&) const {
        return {};
    }
};

//} //end namespace

TileCoverImpl::TileCoverImpl(int32_t z, const Geometry<double>& geom, bool project): max_y(1 << z) {
    ToFeatureType toFeatureType;
    closed_geom = apply_visitor(toFeatureType, geom) == FeatureType::Polygon;

    // Build edge table
    ToEdges te(z, project);
    std::vector<edge_list> edgeLists = apply_visitor(te, geom);
    for(auto& edges: edgeLists) {
        edge_table table;
        build_edge_table(edges, max_y, table, closed_geom);
        et.push_back(table);
    }
    reset();
}

void TileCoverImpl::reset() {
    current_edge_table = et.begin();
    if (current_edge_table != et.end()) {
        aet = current_edge_table->begin()->second;
        current_y = current_edge_table->begin()->first;
    }
}

bool TileCoverImpl::next() {
    return current_edge_table != et.end() && aet.size() != 0 && current_y < max_y;
}

bool TileCoverImpl::scanRow(ScanLine& scanCover) {
    if (!next()) { return false; }

    auto xps = util::scan_row(current_y, aet);
    auto p = xps.begin();

    while (p != xps.end()) {
        auto x_min = p->first;
        // For closed geometries, consider odd even pairs of tile ranges
        if (closed_geom) { p++; }
        auto x_max = p++->second;
        scanCover(x_min, x_max, current_y);
    }

    // Move to the next row
    current_y++;

    // Update AET for next row
    auto nextRow = current_edge_table->find(current_y);
    if (nextRow != current_edge_table->end()) {
        aet.insert(aet.end(), nextRow->second.begin(), nextRow->second.end());
    } else if (aet.size() == 0){
        current_edge_table++;
        if (current_edge_table == et.end()) return false;
        aet = current_edge_table->begin()->second;
        current_y = current_edge_table->begin()->first;
    }

    return next();
}

} // namespace util
} // namespace mbgl
