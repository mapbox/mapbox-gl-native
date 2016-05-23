#include <mbgl/tile/vector_tile_source.hpp>
#include <mbgl/tile/file_based_tile_source_impl.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/vector_tile.hpp>

namespace mbgl {

VectorTileSource::VectorTileSource(GeometryTileData& tileData_,
                                   const Resource& resource_,
                                   FileSource& fileSource_)
    : FileBasedTileSource<GeometryTileSource, VectorTileSource>(tileData_, resource_, fileSource_) {
}

std::unique_ptr<GeometryTile> VectorTileSource::parseData(std::shared_ptr<const std::string> data) {
    return data ? std::make_unique<VectorTile>(data) : nullptr;
}

} // namespace mbgl
