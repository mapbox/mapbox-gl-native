#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {

class Tileset;
class TileSource;

namespace style {
class UpdateParameters;
}

class VectorTileData : public GeometryTileData {
public:
    VectorTileData(const OverscaledTileID&,
                   std::string sourceID,
                   const style::UpdateParameters&,
                   const Tileset&);
    ~VectorTileData();

    void setNecessity(Necessity) final;

private:
    std::unique_ptr<TileSource> tileSource;
};

} // namespace mbgl
