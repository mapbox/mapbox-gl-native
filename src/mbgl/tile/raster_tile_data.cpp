#include <mbgl/tile/raster_tile_data.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>

using namespace mbgl;

RasterTileData::RasterTileData(const OverscaledTileID& id_,
                               float pixelRatio,
                               const std::string& urlTemplate,
                               Worker& worker_,
                               FileSource& fileSource,
                               const std::function<void(std::exception_ptr)>& callback)
    : TileData(id_),
      worker(worker_) {
    const Resource resource =
        Resource::tile(urlTemplate, pixelRatio, id.canonical.x, id.canonical.y, id.canonical.z);
    req = fileSource.request(resource, [callback, this](Response res) {
        if (res.error) {
            callback(std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            modified = res.modified;
            expires = res.expires;
        } else if (res.noContent) {
            availableData = DataAvailability::All;
            modified = res.modified;
            expires = res.expires;
            workRequest.reset();
            bucket.reset();
            callback(nullptr);
        } else {
            modified = res.modified;
            expires = res.expires;

            workRequest.reset();
            workRequest = worker.parseRasterTile(std::make_unique<RasterBucket>(), res.data, [this, callback] (RasterTileParseResult result) {
                workRequest.reset();

                std::exception_ptr error;
                if (result.is<std::unique_ptr<Bucket>>()) {
                    bucket = std::move(result.get<std::unique_ptr<Bucket>>());
                } else {
                    error = result.get<std::exception_ptr>();
                    bucket.reset();
                }

                availableData = DataAvailability::All;

                callback(error);
            });
        }
    });
}

RasterTileData::~RasterTileData() {
    cancel();
}

Bucket* RasterTileData::getBucket(const style::Layer&) {
    return bucket.get();
}

void RasterTileData::cancel() {
    req = nullptr;
    workRequest.reset();
}
