#include <mbgl/tile/vector_tile_data.hpp>
#include <mbgl/tile/file_based_tile_source.hpp>
#include <mbgl/tile/file_based_tile_source_impl.hpp>
#include <mbgl/tile/tile_source.hpp>
#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/async_request.hpp>

namespace mbgl {

VectorTileData::VectorTileData(const OverscaledTileID& overscaledTileID,
                               std::string sourceID,
                               const style::UpdateParameters& parameters,
                               const Tileset& tileset)
    : GeometryTileData(overscaledTileID, sourceID, parameters.style, parameters.mode) {
    assert(!tileset.tiles.empty());
    const auto resource = Resource::tile(
        tileset.tiles.at(0), parameters.pixelRatio, overscaledTileID.canonical.x,
        overscaledTileID.canonical.y, overscaledTileID.canonical.z);
    tileSource = std::make_unique<FileBasedTileSource<GeometryTileSource, VectorTileData>>(
        *this, resource, parameters.fileSource);
}

VectorTileData::~VectorTileData() = default;

void VectorTileData::setNecessity(Necessity necessity) {
    tileSource->setNecessity(static_cast<TileSource::Necessity>(necessity));
}

std::unique_ptr<GeometryTile> VectorTileData::parseData(std::shared_ptr<const std::string> data) {
    return data ? std::make_unique<VectorTile>(data) : nullptr;
}

} // namespace mbgl
