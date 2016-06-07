#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {

class Tileset;
class TileSource;
class GeometryTile;

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

    static std::unique_ptr<GeometryTile> parseData(std::shared_ptr<const std::string>);

private:
    std::unique_ptr<TileSource> tileSource;
};

} // namespace mbgl
