#include <mbgl/style/sources/vector_source_impl.hpp>
#include <mbgl/tile/vector_tile.hpp>

namespace mbgl {
namespace style {

std::unique_ptr<VectorSource> VectorSource::Impl::parse(std::string id, const JSValue& value) {
    optional<variant<std::string, Tileset>> urlOrTileset = TileSourceImpl::parseURLOrTileset(value);
    if (!urlOrTileset) {
        return nullptr;
    }
    return std::make_unique<VectorSource>(std::move(id), std::move(*urlOrTileset));
}

VectorSource::Impl::Impl(std::string id_, Source& base_, variant<std::string, Tileset> urlOrTileset_)
    : TileSourceImpl(SourceType::Vector, std::move(id_), base_, std::move(urlOrTileset_), util::tileSize) {
}

std::unique_ptr<Tile> VectorSource::Impl::createTile(const OverscaledTileID& tileID,
                                                     const UpdateParameters& parameters) {
    return std::make_unique<VectorTile>(tileID, base.getID(), parameters, tileset);
}

} // namespace style
} // namespace mbgl
