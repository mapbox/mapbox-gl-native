#pragma once

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/tile_loader.hpp>

namespace mbgl {

class Tileset;

namespace style {
class UpdateParameters;
} // namespace style

class VectorTile : public GeometryTile {
public:
    VectorTile(const OverscaledTileID&,
               std::string sourceID,
               const style::UpdateParameters&,
               const Tileset&);

    void firstParseFinished(bool succeed) final;
    void setNecessity(Necessity) final;
    void setData(const Response& response);

private:
    optional<Response> response;
    TileLoader<VectorTile> loader;
};

} // namespace mbgl
