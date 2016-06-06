#include <mbgl/tile/vector_tile_data.hpp>
#include <mbgl/tile/vector_tile_source.hpp>
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
    setTileSource(
        std::make_unique<VectorTileSource>(*this, resource, parameters.fileSource));
}

} // namespace mbgl
