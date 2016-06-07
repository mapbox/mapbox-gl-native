#pragma once

#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile_source.hpp>

namespace mbgl {

class Tileset;
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

    void setNecessity(Necessity) final;
    void setData(std::shared_ptr<const std::string> data,
                 optional<Timestamp> modified,
                 optional<Timestamp> expires);

private:
    TileSource<VectorTileData> tileSource;
};

} // namespace mbgl
