#include <mbgl/tile/image_tile_source.hpp>
#include <mbgl/tile/file_based_tile_source_impl.hpp>
#include <mbgl/tile/raster_tile_data.hpp>

namespace mbgl {

ImageTileSource::ImageTileSource(RasterTileData& tileData_,
                                 const Resource& resource_,
                                 FileSource& fileSource_)
    : FileBasedTileSource<RasterTileSource, ImageTileSource>(tileData_, resource_, fileSource_) {
}

} // namespace mbgl
