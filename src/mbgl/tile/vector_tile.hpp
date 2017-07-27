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

    void setNecessity(Necessity) final;

    void setError(std::exception_ptr, bool complete);
    void setData(optional<std::shared_ptr<const std::string>> data,
                 optional<Timestamp> modified_,
                 optional<Timestamp> expires_,
                 bool complete);

private:
    TileLoader<VectorTile> loader;
};

} // namespace mbgl
