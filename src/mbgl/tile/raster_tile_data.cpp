#include <mbgl/tile/raster_tile_data.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/tile/raster_tile_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>

using namespace mbgl;

RasterTileData::RasterTileData(const OverscaledTileID& id_,
                               std::unique_ptr<RasterTileSource> tileSource_,
                               gl::TexturePool& texturePool_,
                               Worker& worker_,
                               const std::function<void(std::exception_ptr)>& callback)
    : TileData(id_, std::move(tileSource_)),
      texturePool(texturePool_),
      worker(worker_) {
    auto rasterTileSource = reinterpret_cast<RasterTileSource*>(tileSource.get());
    tileRequest = rasterTileSource->monitorTile([callback, this](std::exception_ptr err,
                                                        std::shared_ptr<const std::string> data,
                                                        optional<Timestamp> modified_,
                                                        optional<Timestamp> expires_) {
        if (err) {
            callback(err);
            return;
        }

        modified = modified_;
        expires = expires_;

        if (!data) {
            // This is a 404 response. We're treating these as empty tiles.
            workRequest.reset();
            availableData = DataAvailability::All;
            bucket.reset();
            callback(err);
            return;
        }

        workRequest.reset();
        workRequest = worker.parseRasterTile(std::make_unique<RasterBucket>(texturePool), data, [this, callback] (RasterTileParseResult result) {
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
    });
}

RasterTileData::~RasterTileData() {
    cancel();
}

Bucket* RasterTileData::getBucket(const style::Layer&) {
    return bucket.get();
}

void RasterTileData::cancel() {
    tileRequest.reset();
    workRequest.reset();
}
