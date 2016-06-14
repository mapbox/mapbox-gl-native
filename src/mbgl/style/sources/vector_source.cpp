#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/tile/vector_tile.hpp>

namespace mbgl {
namespace style {

std::unique_ptr<VectorSource> VectorSource::parse(std::string id, const JSValue& value) {
    optional<variant<std::string, Tileset>> urlOrTileset = TileSource::parseURLOrTileset(value);
    if (!urlOrTileset) {
        return nullptr;
    }
    return std::make_unique<VectorSource>(std::move(id), std::move(*urlOrTileset));
}

VectorSource::VectorSource(std::string id_, variant<std::string, Tileset> urlOrTileset_)
    : TileSource(SourceType::Vector, std::move(id_), std::move(urlOrTileset_), util::tileSize) {
}

std::unique_ptr<Tile> VectorSource::createTile(const OverscaledTileID& tileID,
                                               const UpdateParameters& parameters) {
    return std::make_unique<VectorTile>(tileID, id, parameters, tileset);
}

} // namespace style
} // namespace mbgl
