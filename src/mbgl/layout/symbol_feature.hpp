#pragma once

#include <mbgl/text/bidi.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/optional.hpp>

#include <string>

namespace mbgl {

class SymbolFeature {
public:
    GeometryCollection geometry;
    optional<std::u16string> text;
    optional<WritingDirection> writingDirection;
    optional<std::string> icon;
    std::size_t index;
};

} // namespace mbgl
