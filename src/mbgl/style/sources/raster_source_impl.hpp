#pragma once

#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/tile_source_impl.hpp>

namespace mbgl {
namespace style {

class RasterSource::Impl : public TileSourceImpl {
public:
    Impl(std::string id, Source&, variant<std::string, Tileset>, uint16_t tileSize);

private:
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;
};

} // namespace style
} // namespace mbgl
