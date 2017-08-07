#pragma once

#include <mapbox/geometry/wagyu/build_edges.hpp>
#include <mapbox/geometry/wagyu/config.hpp>
#include <mapbox/geometry/wagyu/local_minimum.hpp>
#include <mapbox/geometry/wagyu/local_minimum_util.hpp>

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T1, typename T2>
bool add_linear_ring(mapbox::geometry::linear_ring<T2> const& path_geometry,
                     local_minimum_list<T1>& minima_list,
                     polygon_type p_type) {
    edge_list<T1> new_edges;
    new_edges.reserve(path_geometry.size());
    if (!build_edge_list<T1, T2>(path_geometry, new_edges) || new_edges.empty()) {
        return false;
    }
    add_ring_to_local_minima_list(new_edges, minima_list, p_type);
    return true;
}
}
}
}
