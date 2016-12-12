#pragma once
#include <mapbox/earcut.hpp>

#include <array>
#include <memory>
#include <vector>

template <typename Coord, typename Polygon>
class EarcutTesselator {
public:
    using Vertex = std::array<Coord, 2>;
    using Vertices = std::vector<Vertex>;

    EarcutTesselator(const Polygon &polygon_)
        : polygon(polygon_)
    {
        for (const auto& ring : polygon_) {
            for (const auto& vertex : ring) {
                vertices_.emplace_back(Vertex {{ Coord(std::get<0>(vertex)),
                                                 Coord(std::get<1>(vertex)) }});
            }
        }
    }

    void run() {
        indices_ = mapbox::earcut(polygon);
    }

    std::vector<uint32_t> indices() const {
        return indices_;
    }

    Vertices vertices() const {
        return vertices_;
    }

private:
    const Polygon &polygon;
    Vertices vertices_;
    std::vector<uint32_t> indices_;
};
