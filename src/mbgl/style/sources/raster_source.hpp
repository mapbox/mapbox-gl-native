#pragma once

#include <mbgl/style/tile_source.hpp>

namespace mbgl {
namespace style {

class RasterSource : public TileSource {
public:
    RasterSource(std::string id,
                 std::string url,
                 uint16_t tileSize,
                 std::unique_ptr<Tileset>&&);

private:
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;
};

} // namespace style
} // namespace mbgl
