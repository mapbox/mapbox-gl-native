#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/tile/vector_tile.hpp>

namespace mbgl {
namespace style {

VectorSource::VectorSource(std::string id_,
                           std::string url_,
                           std::unique_ptr<Tileset>&& tileset_)
    : TileSource(SourceType::Vector, std::move(id_), std::move(url_), util::tileSize, std::move(tileset_)) {
}

std::unique_ptr<Tile> VectorSource::createTile(const OverscaledTileID& tileID,
                                               const UpdateParameters& parameters) {
    return std::make_unique<VectorTile>(tileID, id, parameters, *tileset);
}

} // namespace style
} // namespace mbgl
