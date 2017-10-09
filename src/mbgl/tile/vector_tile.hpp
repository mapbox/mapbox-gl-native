#pragma once

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/tile_loader.hpp>

namespace mbgl {

class Tileset;
class TileParameters;

class VectorTile : public GeometryTile {
public:
    VectorTile(const OverscaledTileID&,
               std::string sourceID,
               const TileParameters&,
               const Tileset&);

    void setNecessity(TileNecessity) final;
    void setMetadata(optional<Timestamp> modified, optional<Timestamp> expires);
    void setData(std::shared_ptr<const std::string> data);

private:
    TileLoader<VectorTile> loader;
};

} // namespace mbgl
