#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/tile/raster_tile.hpp>

namespace mbgl {
namespace style {

RasterSource::RasterSource(std::string id_,
                           std::string url_,
                           uint16_t tileSize_,
                           std::unique_ptr<Tileset>&& tileset_)
    : TileSource(SourceType::Raster, std::move(id_), std::move(url_), tileSize_, std::move(tileset_)) {
}

std::unique_ptr<Tile> RasterSource::createTile(const OverscaledTileID& tileID,
                                               const UpdateParameters& parameters) {
    return std::make_unique<RasterTile>(tileID, parameters, *tileset);
}

} // namespace style
} // namespace mbgl
