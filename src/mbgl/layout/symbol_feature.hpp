#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/optional.hpp>

#include <string>

namespace mbgl {

class SymbolFeature {
public:
    GeometryCollection geometry;
    optional<std::u32string> label;
    optional<std::string> sprite;
    std::size_t index;
};

} // namespace mbgl
