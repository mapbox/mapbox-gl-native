#include <mbgl/tile/raster_tile_source.hpp>
#include <mbgl/storage/file_source.hpp>

namespace mbgl {

RasterTileSource::RasterTileSource(const OverscaledTileID& tileID_,
                                   float pixelRatio_,
                                   const std::string& urlTemplate_,
                                   FileSource& fileSource_)
    : tileID(tileID_), pixelRatio(pixelRatio_), urlTemplate(urlTemplate_), fileSource(fileSource_) {
}

std::unique_ptr<AsyncRequest>
RasterTileSource::monitorTile(const Callback& callback) {
    const Resource resource = Resource::tile(urlTemplate, pixelRatio, tileID.canonical.x,
                                             tileID.canonical.y, tileID.canonical.z);
    return fileSource.request(resource, [callback, this](Response res) {
        if (res.error) {
            callback(std::make_exception_ptr(std::runtime_error(res.error->message)), nullptr,
                     res.modified, res.expires);
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            callback(nullptr, nullptr, res.modified, res.expires);
        } else {
            callback(nullptr, res.data, res.modified, res.expires);
        }
    });
}

} // namespace mbgl
