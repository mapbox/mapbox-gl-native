#include <mbgl/tile/vector_tile_source.hpp>
#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/storage/file_source.hpp>

namespace mbgl {

VectorTileSource::VectorTileSource(const Resource& resource_, FileSource& fileSource_)
    : resource(resource_), fileSource(fileSource_) {
}

std::unique_ptr<AsyncRequest> VectorTileSource::monitorTile(const Callback& callback) {
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
