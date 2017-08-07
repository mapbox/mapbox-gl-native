#pragma once

#include <list>

#include <mapbox/geometry/point.hpp>

#include <mapbox/geometry/wagyu/config.hpp>
#include <mapbox/geometry/wagyu/edge.hpp>
#include <mapbox/geometry/wagyu/ring.hpp>

#ifdef DEBUG
#include <iostream>
#endif

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
struct bound {

    edge_list<T> edges;
    edge_list_itr<T> current_edge;
    edge_list_itr<T> next_edge;
    mapbox::geometry::point<T> last_point;
    ring_ptr<T> ring;
    bound_ptr<T> maximum_bound; // the bound who's maximum connects with this bound
    double current_x;
    std::size_t pos;
    std::int32_t winding_count;
    std::int32_t winding_count2; // winding count of the opposite polytype
    std::int8_t winding_delta;   // 1 or -1 depending on winding direction - 0 for linestrings
    polygon_type poly_type;
    edge_side side; // side only refers to current side of solution poly

    bound() noexcept
        : edges(),
          current_edge(edges.end()),
          next_edge(edges.end()),
          last_point({ 0, 0 }),
          ring(nullptr),
          maximum_bound(nullptr),
          current_x(0.0),
          pos(0),
          winding_count(0),
          winding_count2(0),
          winding_delta(0),
          poly_type(polygon_type_subject),
          side(edge_left) {
    }

    bound(bound<T>&& b) noexcept
        : edges(std::move(b.edges)),
          current_edge(std::move(b.current_edge)),
          next_edge(std::move(b.next_edge)),
          last_point(std::move(b.last_point)),
          ring(std::move(b.ring)),
          maximum_bound(std::move(b.maximum_bound)),
          current_x(std::move(b.current_x)),
          pos(std::move(b.pos)),
          winding_count(std::move(b.winding_count)),
          winding_count2(std::move(b.winding_count2)),
          winding_delta(std::move(b.winding_delta)),
          poly_type(std::move(b.poly_type)),
          side(std::move(b.side)) {
    }

    bound(bound<T>const& b) = delete;
    bound<T>& operator=(bound<T> const&) = delete;

};

#ifdef DEBUG

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     const bound<T>& bnd) {
    out << "    Bound: " << &bnd << std::endl;
    out << "        current_x: " << bnd.current_x << std::endl;
    out << "        last_point: " << bnd.last_point.x << ", " << bnd.last_point.y << std::endl;
    out << *(bnd.current_edge);
    out << "        winding count: " << bnd.winding_count << std::endl;
    out << "        winding_count2: " << bnd.winding_count2 << std::endl;
    out << "        winding_delta: " << static_cast<int>(bnd.winding_delta) << std::endl;
    out << "        maximum_bound: " << bnd.maximum_bound << std::endl;
    if (bnd.side == edge_left) {
        out << "        side: left" << std::endl;
    } else {
        out << "        side: right" << std::endl;
    }
    out << "        ring: " << bnd.ring << std::endl;
    if (bnd.ring) {
        out << "        ring index: " << bnd.ring->ring_index << std::endl;
    }
    return out;
}

#endif
}
}
}
