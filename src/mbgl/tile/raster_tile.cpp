#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>

namespace mbgl {

RasterTile::RasterTile(const OverscaledTileID& id_,
                       const style::UpdateParameters& parameters,
                       const Tileset& tileset)
    : Tile(id_),
      worker(parameters.worker),
      loader(*this, id_, parameters, tileset) {
}

RasterTile::~RasterTile() = default;

void RasterTile::setError(std::exception_ptr err) {
    observer->onTileError(*this, err);
}

void RasterTile::setData(std::shared_ptr<const std::string> data,
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
    workRequest = worker.parseRasterTile(std::make_unique<RasterBucket>(), data, [this] (RasterTileParseResult result) {
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

Bucket* RasterTile::getBucket(const style::Layer&) {
    return bucket.get();
}

void RasterTile::setNecessity(Necessity necessity) {
    loader.setNecessity(necessity);
}

void RasterTile::cancel() {
    workRequest.reset();
}

} // namespace mbgl
