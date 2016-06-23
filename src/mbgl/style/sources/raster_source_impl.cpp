#include <mbgl/style/sources/raster_source_impl.hpp>
#include <mbgl/tile/raster_tile.hpp>

namespace mbgl {
namespace style {

RasterSource::Impl::Impl(std::string id_, Source& base_,
                         variant<std::string, Tileset> urlOrTileset_,
                         uint16_t tileSize_)
    : TileSourceImpl(SourceType::Raster, std::move(id_), base_, std::move(urlOrTileset_), tileSize_) {
}

std::unique_ptr<Tile> RasterSource::Impl::createTile(const OverscaledTileID& tileID,
                                               const UpdateParameters& parameters) {
    return std::make_unique<RasterTile>(tileID, parameters, tileset);
}

} // namespace style
} // namespace mbgl
