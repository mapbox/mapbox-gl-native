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
          
     const CanonicalTileID canonical = id_.canonical;
     const u_int8_t dim = pow(2, canonical.z);
     
     const u_int32_t px = (canonical.x - 1 + dim) % dim;
     const int16_t pxw = (canonical.x == 0) ? id_.wrap - 1 : id_.wrap;
     const u_int32_t nx = (canonical.x + 1 + dim) % dim;
     const int16_t nxw = (canonical.x + 1 == dim) ? id_.wrap + 1 : id_.wrap;
     
     // adjacent tiles
     neighboringTiles.emplace(OverscaledTileID(id_.overscaledZ, pxw, canonical.z, px, canonical.y), false);
     neighboringTiles.emplace(OverscaledTileID(id_.overscaledZ, nxw, canonical.z, nx, canonical.y), false);
     
     // upper bordering tiles
     if (canonical.y > 0) {
         neighboringTiles.emplace(OverscaledTileID(id_.overscaledZ, pxw, canonical.z, px, canonical.y - 1), false);
         neighboringTiles.emplace(OverscaledTileID(id_.overscaledZ, id_.wrap, canonical.z, canonical.x, canonical.y - 1), false);
         neighboringTiles.emplace(OverscaledTileID(id_.overscaledZ, nxw, canonical.z, nx, canonical.y - 1), false);
     }
     
     // lower bordering tiles
     if (canonical.y + 1 < dim) {
         neighboringTiles.emplace(OverscaledTileID(id_.overscaledZ, pxw, canonical.z, px, canonical.y + 1), false);
         neighboringTiles.emplace(OverscaledTileID(id_.overscaledZ, id_.wrap, canonical.z, canonical.x, canonical.y + 1), false);
         neighboringTiles.emplace(OverscaledTileID(id_.overscaledZ, nxw, canonical.z, nx, canonical.y + 1), false);
     }
}

RasterDEMTile::~RasterDEMTile() = default;

void RasterDEMTile::cancel() {
}

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

void RasterDEMTile::setMask(TileMask&& mask) {
    if (bucket) {
        bucket->setMask(std::move(mask));
    }
}

void RasterDEMTile::setNecessity(TileNecessity necessity) {
    loader.setNecessity(necessity);
}

} // namespace mbgl
