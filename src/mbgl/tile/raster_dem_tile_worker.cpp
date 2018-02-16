#include <mbgl/tile/raster_dem_tile_worker.hpp>
#include <mbgl/tile/raster_dem_tile.hpp>
#include <mbgl/renderer/buckets/hillshade_bucket.hpp>
#include <mbgl/actor/actor.hpp>
#include <mbgl/util/premultiply.hpp>

namespace mbgl {

RasterDEMTileWorker::RasterDEMTileWorker(ActorRef<RasterDEMTileWorker>, ActorRef<RasterDEMTile> parent_)
    : parent(std::move(parent_)) {
}

void RasterDEMTileWorker::parse(std::shared_ptr<const std::string> data, uint64_t correlationID, Tileset::DEMEncoding encoding) {
    if (!data) {
        parent.invoke(&RasterDEMTile::onParsed, nullptr, correlationID); // No data; empty tile.
        return;
    }

    try {
        auto bucket = std::make_unique<HillshadeBucket>(decodeImage(*data), encoding);
        parent.invoke(&RasterDEMTile::onParsed, std::move(bucket), correlationID);
    } catch (...) {
        parent.invoke(&RasterDEMTile::onError, std::current_exception(), correlationID);
    }
}

} // namespace mbgl
