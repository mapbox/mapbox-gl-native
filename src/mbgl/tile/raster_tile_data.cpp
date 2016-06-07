#include <mbgl/tile/raster_tile_data.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/tile/tile_data_observer.hpp>
#include <mbgl/tile/tile_source_impl.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>

using namespace mbgl;

RasterTileData::RasterTileData(const OverscaledTileID& id_,
                               const style::UpdateParameters& parameters,
                               const Tileset& tileset)
    : TileData(id_),
      texturePool(parameters.texturePool),
      worker(parameters.worker),
      tileSource(*this, id_, parameters, tileset) {
}

void RasterTileData::setError(std::exception_ptr err) {
    observer->onTileError(*this, err);
}

void RasterTileData::setData(std::shared_ptr<const std::string> data,
                             optional<Timestamp> modified_,
                             optional<Timestamp> expires_) {
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

Bucket* RasterTileData::getBucket(const style::Layer&) {
    return bucket.get();
}

void RasterTileData::setNecessity(Necessity necessity) {
    tileSource.setNecessity(static_cast<TileSource<RasterTileData>::Necessity>(necessity));
}

void RasterTileData::cancel() {
    workRequest.reset();
}
