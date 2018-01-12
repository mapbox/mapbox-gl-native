#include <mbgl/tile/raster_dem_tile.hpp>
#include <mbgl/tile/raster_dem_tile_worker.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/buckets/hillshade_bucket.hpp>
#include <mbgl/actor/scheduler.hpp>

namespace mbgl {

RasterDEMTile::RasterDEMTile(const OverscaledTileID& id_,
                       const TileParameters& parameters,
                       const Tileset& tileset)
    : Tile(id_),
      loader(*this, id_, parameters, tileset),
      mailbox(std::make_shared<Mailbox>(*Scheduler::GetCurrent())),
      worker(parameters.workerScheduler,
             ActorRef<RasterDEMTile>(*this, mailbox)) {

    if ( id.canonical.y == 0 ){
        // this tile doesn't have upper neighboring tiles so marked those as backfilled
        neighboringTiles = neighboringTiles | DEMTileNeighbors::NoUpper;
    }

    if (id.canonical.y + 1 == std::pow(2, id.canonical.z)){
        // this tile doesn't have lower neighboring tiles so marked those as backfilled
        neighboringTiles = neighboringTiles | DEMTileNeighbors::NoLower;
    }
}

RasterDEMTile::~RasterDEMTile() = default;

void RasterDEMTile::setError(std::exception_ptr err) {
    loaded = true;
    observer->onTileError(*this, err);
}

void RasterDEMTile::setMetadata(optional<Timestamp> modified_, optional<Timestamp> expires_) {
    modified = modified_;
    expires = expires_;
}

void RasterDEMTile::setData(std::shared_ptr<const std::string> data) {
    pending = true;
    ++correlationID;
    worker.invoke(&RasterDEMTileWorker::parse, data, correlationID);
}

void RasterDEMTile::onParsed(std::unique_ptr<HillshadeBucket> result, const uint64_t resultCorrelationID) {
    bucket = std::move(result);
    loaded = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }
    renderable = bucket ? true : false;
    observer->onTileChanged(*this);
}

void RasterDEMTile::onError(std::exception_ptr err, const uint64_t resultCorrelationID) {
    loaded = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }
    observer->onTileError(*this, err);
}

void RasterDEMTile::upload(gl::Context& context) {
    if (bucket) {
        bucket->upload(context);
    }
}


Bucket* RasterDEMTile::getBucket(const style::Layer::Impl&) const {
    return bucket.get();
}

HillshadeBucket* RasterDEMTile::getBucket() const {
    return bucket.get();
}


void RasterDEMTile::setMask(TileMask&& mask) {
    if (bucket) {
        bucket->setMask(std::move(mask));
    }
}

void RasterDEMTile::setNecessity(TileNecessity necessity) {
    loader.setNecessity(necessity);
}

} // namespace mbgl
