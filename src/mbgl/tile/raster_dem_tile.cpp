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

    encoding = tileset.encoding;
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
    worker.invoke(&RasterDEMTileWorker::parse, data, correlationID, encoding);
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

void RasterDEMTile::backfillBorder(const RasterDEMTile& borderTile, const DEMTileNeighbors mask) {
    int32_t dx = borderTile.id.canonical.x - id.canonical.x;
    const int8_t dy = borderTile.id.canonical.y - id.canonical.y;
    const uint32_t dim = pow(2, id.canonical.z);
    if (dx == 0 && dy == 0) return;
    if (std::abs(dy) > 1) return;
    // neighbor is in another world wrap
    if (std::abs(dx) > 1) {
        if (std::abs(int(dx + dim)) == 1) {
            dx += dim;
        } else if (std::abs(int(dx - dim)) == 1) {
            dx -= dim;
        }
    }
    const HillshadeBucket* borderBucket = borderTile.getBucket();
    if (borderBucket) {
        const DEMData& borderDEM = borderBucket->getDEMData();
        DEMData& tileDEM = bucket->getDEMData();

        tileDEM.backfillBorder(borderDEM, dx, dy);
        // update the bitmask to indicate that this tiles have been backfilled by flipping the relevant bit
        this->neighboringTiles = this->neighboringTiles | mask;
        // mark HillshadeBucket.prepared as false so it runs through the prepare render pass
        // with the new texture data we just backfilled
        bucket->setPrepared(false);
    }
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
