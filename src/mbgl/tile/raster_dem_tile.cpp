#include <mbgl/tile/raster_dem_tile.hpp>

#include <mbgl/actor/scheduler.hpp>
#include <mbgl/renderer/buckets/hillshade_bucket.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/tile_render_data.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/tile/raster_dem_tile_worker.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <utility>

namespace mbgl {

RasterDEMTile::RasterDEMTile(const OverscaledTileID& id_,
                       const TileParameters& parameters,
                       const Tileset& tileset)
    : Tile(Kind::RasterDEM, id_),
      loader(*this, id_, parameters, tileset),
      mailbox(std::make_shared<Mailbox>(*Scheduler::GetCurrent())),
      worker(Scheduler::GetBackground(),
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

std::unique_ptr<TileRenderData> RasterDEMTile::createRenderData() {
    return std::make_unique<SharedBucketTileRenderData<HillshadeBucket>>(bucket);
}

void RasterDEMTile::setError(std::exception_ptr err) {
    loaded = true;
    observer->onTileError(*this, std::move(err));
}

void RasterDEMTile::setMetadata(optional<Timestamp> modified_, optional<Timestamp> expires_) {
    modified = std::move(modified_);
    expires = std::move(expires_);
}

void RasterDEMTile::setData(const std::shared_ptr<const std::string>& data) {
    pending = true;
    ++correlationID;
    worker.self().invoke(&RasterDEMTileWorker::parse, data, correlationID, encoding);
}

void RasterDEMTile::onParsed(std::unique_ptr<HillshadeBucket> result, const uint64_t resultCorrelationID) {
    bucket = std::move(result);
    loaded = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }
    renderable = static_cast<bool>(bucket);
    observer->onTileChanged(*this);
}

void RasterDEMTile::onError(std::exception_ptr err, const uint64_t resultCorrelationID) {
    loaded = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }
    observer->onTileError(*this, std::move(err));
}

bool RasterDEMTile::layerPropertiesUpdated(const Immutable<style::LayerProperties>&) {
    return bool(bucket);
}

HillshadeBucket* RasterDEMTile::getBucket() const {
    return bucket.get();
}

void RasterDEMTile::backfillBorder(const RasterDEMTile& borderTile, const DEMTileNeighbors mask) {
    int32_t dx = static_cast<int32_t>(borderTile.id.canonical.x) - static_cast<int32_t>(id.canonical.x);
    const auto dy =
        static_cast<int8_t>(static_cast<int32_t>(borderTile.id.canonical.y) - static_cast<int32_t>(id.canonical.y));
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

void RasterDEMTile::setMinimumUpdateInterval(Duration interval) {
    loader.setMinimumUpdateInterval(interval);
}

} // namespace mbgl
