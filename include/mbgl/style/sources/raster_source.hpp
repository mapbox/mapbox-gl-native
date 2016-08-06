#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {
namespace style {

class RasterSource : public Source {
public:
    RasterSource(std::string id, variant<std::string, Tileset> urlOrTileset, uint16_t tileSize);

    // Private implementation

    class Impl;
};

} // namespace style
} // namespace mbgl
