#pragma once

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/tile_source.hpp>

namespace mbgl {

class Tileset;

namespace style {
class UpdateParameters;
}

class VectorTile : public GeometryTile {
public:
    VectorTile(const OverscaledTileID&,
               std::string sourceID,
               const style::UpdateParameters&,
               const Tileset&);

    void setNecessity(Necessity) final;
    void setData(std::shared_ptr<const std::string> data,
                 optional<Timestamp> modified,
                 optional<Timestamp> expires);

private:
    TileSource<VectorTile> tileSource;
};

} // namespace mbgl
