#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/optional.hpp>

#include <string>

namespace mbgl {

class SymbolFeature {
public:
    FeatureType type;
    GeometryCollection geometry;
    optional<std::u16string> text;
    optional<std::string> icon;
    std::size_t index;
};

} // namespace mbgl
