#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/tile/raster_tile_worker.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/actor/scheduler.hpp>

namespace mbgl {

RasterTile::RasterTile(const OverscaledTileID& id_,
                       const TileParameters& parameters,
                       const Tileset& tileset)
    : Tile(id_),
      loader(*this, id_, parameters, tileset),
      mailbox(std::make_shared<Mailbox>(*Scheduler::GetCurrent())),
      worker(parameters.workerScheduler,
             ActorRef<RasterTile>(*this, mailbox)) {
}

RasterTile::~RasterTile() = default;

void RasterTile::setError(std::exception_ptr err) {
    loaded = true;
    observer->onTileError(*this, err);
}

void RasterTile::setMetadata(optional<Timestamp> modified_, optional<Timestamp> expires_) {
    modified = modified_;
    expires = expires_;
}

void RasterTile::setData(std::shared_ptr<const std::string> data) {
    pending = true;
    ++correlationID;
    worker.invoke(&RasterTileWorker::parse, data, correlationID);
}

void RasterTile::onParsed(std::unique_ptr<RasterBucket> result, const uint64_t resultCorrelationID) {
    bucket = std::move(result);
    loaded = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }
    renderable = bucket ? true : false;
    observer->onTileChanged(*this);
}

void RasterTile::onError(std::exception_ptr err, const uint64_t resultCorrelationID) {
    loaded = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }
    observer->onTileError(*this, err);
}

void RasterTile::upload(gl::Context& context) {
    if (bucket) {
        bucket->upload(context);
    }
}

Bucket* RasterTile::getBucket(const style::Layer::Impl&) const {
    return bucket.get();
}

void RasterTile::setMask(TileMask&& mask) {
    if (bucket) {
        bucket->setMask(std::move(mask));
    }
}

void RasterTile::setNecessity(TileNecessity necessity) {
    loader.setNecessity(necessity);
}

} // namespace mbgl
