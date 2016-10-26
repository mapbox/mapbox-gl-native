#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/tile/raster_tile_worker.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/util/run_loop.hpp>

namespace mbgl {

RasterTile::RasterTile(const OverscaledTileID& id_,
                       const style::UpdateParameters& parameters,
                       const Tileset& tileset)
    : Tile(id_),
      loader(*this, id_, parameters, tileset),
      mailbox(std::make_shared<Mailbox>(*util::RunLoop::Get())),
      worker(parameters.workerScheduler,
             ActorRef<RasterTile>(*this, mailbox)) {
}

RasterTile::~RasterTile() = default;

void RasterTile::cancel() {
}

void RasterTile::setError(std::exception_ptr err) {
    observer->onTileError(*this, err);
}

void RasterTile::setData(std::shared_ptr<const std::string> data,
                             optional<Timestamp> modified_,
                             optional<Timestamp> expires_) {
    modified = modified_;
    expires = expires_;
    worker.invoke(&RasterTileWorker::parse, data);
}

void RasterTile::onParsed(std::unique_ptr<Bucket> result) {
    bucket = std::move(result);
    availableData = DataAvailability::All;
    observer->onTileChanged(*this);
}

void RasterTile::onError(std::exception_ptr err) {
    bucket.reset();
    availableData = DataAvailability::All;
    observer->onTileError(*this, err);
}

Bucket* RasterTile::getBucket(const style::Layer&) {
    return bucket.get();
}

void RasterTile::setNecessity(Necessity necessity) {
    loader.setNecessity(necessity);
}

} // namespace mbgl
