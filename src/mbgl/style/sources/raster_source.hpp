#pragma once

#include <mbgl/style/tile_source.hpp>

namespace mbgl {
namespace style {

class RasterSource : public TileSource {
public:
    static std::unique_ptr<RasterSource> parse(std::string id, const JSValue&);

    RasterSource(std::string id, variant<std::string, Tileset>, uint16_t tileSize);

private:
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;
};

} // namespace style
} // namespace mbgl
