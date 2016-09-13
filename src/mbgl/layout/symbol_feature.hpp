#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>

#include <string>

namespace mbgl {

class SymbolFeature {
public:
    GeometryCollection geometry;
    std::u32string label;
    std::string sprite;
    std::size_t index;
};

} // namespace mbgl
