#include <mbgl/tile/vector_tile_source.hpp>
#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/storage/file_source.hpp>

namespace mbgl {

VectorTileSource::VectorTileSource(const OverscaledTileID& tileID_,
                                   float pixelRatio_,
                                   const std::string& urlTemplate_,
                                   FileSource& fileSource_)
    : tileID(tileID_), pixelRatio(pixelRatio_), urlTemplate(urlTemplate_), fileSource(fileSource_) {
}

std::unique_ptr<AsyncRequest>
VectorTileSource::monitorTile(const GeometryTileSource::Callback& callback) {
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
            callback(nullptr, std::make_unique<VectorTile>(res.data), res.modified, res.expires);
        }
    });
}

} // namespace mbgl
