#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>
#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/tile/vector_tile_data.hpp>
#include <utility>

namespace mbgl {

VectorTile::VectorTile(const OverscaledTileID& id_,
                       std::string sourceID_,
                       const TileParameters& parameters,
                       const Tileset& tileset)
    : GeometryTile(id_, std::move(sourceID_), parameters), loader(*this, id_, parameters, tileset) {}

void VectorTile::setNecessity(TileNecessity necessity) {
    loader.setNecessity(necessity);
}

void VectorTile::setUpdateParameters(const TileUpdateParameters& params) {
    loader.setUpdateParameters(params);
}

void VectorTile::setMetadata(optional<Timestamp> modified_, optional<Timestamp> expires_) {
    modified = std::move(modified_);
    expires = std::move(expires_);
}

void VectorTile::setData(const std::shared_ptr<const std::string>& data_) {
    GeometryTile::setData(data_ ? std::make_unique<VectorTileData>(data_) : nullptr);
}

} // namespace mbgl
