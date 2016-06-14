#include <mbgl/tile/vector_tile_data.hpp>
#include <mbgl/tile/tile_source.hpp>
#include <mbgl/tile/tile_source_impl.hpp>
#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/util/async_request.hpp>

namespace mbgl {

VectorTileData::VectorTileData(const OverscaledTileID& id_,
                               std::string sourceID,
                               const style::UpdateParameters& parameters,
                               const Tileset& tileset)
    : GeometryTileData(id_, sourceID, parameters.style, parameters.mode),
      tileSource(*this, id_, parameters, tileset) {
}

VectorTileData::~VectorTileData() = default;

void VectorTileData::setNecessity(Necessity necessity) {
    tileSource.setNecessity(static_cast<TileSource<VectorTileData>::Necessity>(necessity));
}

std::unique_ptr<GeometryTile> VectorTileData::parseData(std::shared_ptr<const std::string> data) {
    return data ? std::make_unique<VectorTile>(data) : nullptr;
}

} // namespace mbgl
