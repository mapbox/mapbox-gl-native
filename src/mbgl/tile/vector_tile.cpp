#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/tile/vector_tile_data.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>
#include <mbgl/style/update_parameters.hpp>

#include <map>
#include <unordered_map>
#include <functional>
#include <utility>

namespace mbgl {

VectorTile::VectorTile(const OverscaledTileID& id_,
                       std::string sourceID_,
                       const style::UpdateParameters& parameters,
                       const Tileset& tileset)
    : GeometryTile(id_, sourceID_, parameters.style, parameters.mode),
      loader(*this, id_, parameters, tileset) {
}

void VectorTile::setNecessity(Necessity necessity) {
    loader.setNecessity(necessity);
}

void VectorTile::setData(std::shared_ptr<const std::string> data_,
                         optional<Timestamp> modified_,
                         optional<Timestamp> expires_) {
    modified = modified_;
    expires = expires_;

    GeometryTile::setData(data_ ? std::make_unique<VectorTileData>(data_) : nullptr);
}

} // namespace mbgl
