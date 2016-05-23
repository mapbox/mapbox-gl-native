#include <mbgl/tile/raster_tile_data.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/tile/tile_data_observer.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>

using namespace mbgl;

RasterTileData::RasterTileData(const OverscaledTileID& id_,
                               gl::TexturePool& texturePool_,
                               Worker& worker_)
    : TileData(id_),
      texturePool(texturePool_),
      worker(worker_) {
}

void RasterTileData::setData(std::exception_ptr err,
                               std::shared_ptr<const std::string> data,
                               optional<Timestamp> modified_,
                               optional<Timestamp> expires_) {
    if (err) {
        observer->onTileError(*this, err);
        return;
    }

    modified = modified_;
    expires = expires_;

    if (!data) {
        // This is a 404 response. We're treating these as empty tiles.
        workRequest.reset();
        availableData = DataAvailability::All;
        bucket.reset();
        observer->onTileLoaded(*this, true);
        return;
    }

    workRequest.reset();
    workRequest = worker.parseRasterTile(std::make_unique<RasterBucket>(texturePool), data, [this] (RasterTileParseResult result) {
        workRequest.reset();

        availableData = DataAvailability::All;

        if (result.is<std::unique_ptr<Bucket>>()) {
            bucket = std::move(result.get<std::unique_ptr<Bucket>>());
            observer->onTileLoaded(*this, true);
        } else {
            bucket.reset();
            observer->onTileError(*this, result.get<std::exception_ptr>());
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
    workRequest.reset();
}
