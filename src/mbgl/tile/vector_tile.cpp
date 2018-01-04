#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/tile/vector_tile_data.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>
#include <mbgl/renderer/tile_parameters.hpp>

namespace mbgl {

VectorTile::VectorTile(const OverscaledTileID& id_,
                       std::string sourceID_,
                       const TileParameters& parameters,
                       const Tileset& tileset)
    : GeometryTile(id_, sourceID_, parameters), loader(*this, id_, parameters, tileset) {
}

void VectorTile::setNecessity(TileNecessity necessity) {
    loader.setNecessity(necessity);
}

void VectorTile::setMetadata(optional<Timestamp> modified_, optional<Timestamp> expires_) {
    modified = modified_;
    expires = expires_;
}

void VectorTile::setData(std::shared_ptr<const std::string> data_) {
    GeometryTile::setData(data_ ? std::make_unique<VectorTileData>(data_) : nullptr);
}

} // namespace mbgl
