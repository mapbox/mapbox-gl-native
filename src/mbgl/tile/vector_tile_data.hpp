#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {

class Tileset;

namespace style {
class UpdateParameters;
}

class VectorTileData : public GeometryTileData {
public:
    VectorTileData(const OverscaledTileID&,
                   std::string sourceID,
                   const style::UpdateParameters&,
                   const Tileset&);
};

} // namespace mbgl
