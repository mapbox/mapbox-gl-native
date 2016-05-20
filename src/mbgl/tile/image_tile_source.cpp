#include <mbgl/tile/image_tile_source.hpp>
#include <mbgl/tile/raster_tile_data.hpp>
#include <mbgl/storage/file_source.hpp>

namespace mbgl {

ImageTileSource::ImageTileSource(RasterTileData& tileData_,
                                 const Resource& resource_,
                                 FileSource& fileSource_)
    : RasterTileSource(tileData_), resource(resource_), fileSource(fileSource_) {
    request = fileSource.request(resource, [this](Response res) {
        if (res.error) {
            tileData.setData(std::make_exception_ptr(std::runtime_error(res.error->message)),
                             nullptr, res.modified, res.expires);
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            tileData.setData(nullptr, nullptr, res.modified, res.expires);
        } else {
            tileData.setData(nullptr, res.data, res.modified, res.expires);
        }
    });
}

} // namespace mbgl
