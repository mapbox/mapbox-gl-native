#include <mbgl/tile/raster_tile_worker.hpp>
#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/actor/actor.hpp>
#include <mbgl/util/premultiply.hpp>

namespace mbgl {

RasterTileWorker::RasterTileWorker(ActorRef<RasterTileWorker>, ActorRef<RasterTile> parent_)
    : parent(std::move(parent_)) {
}

void RasterTileWorker::parse(std::shared_ptr<const std::string> data, uint64_t correlationID) {
    if (!data) {
        parent.invoke(&RasterTile::onParsed, nullptr, correlationID); // No data; empty tile.
        return;
    }

    try {
        auto bucket = std::make_unique<RasterBucket>(decodeImage(*data));
        parent.invoke(&RasterTile::onParsed, std::move(bucket), correlationID);
    } catch (...) {
        parent.invoke(&RasterTile::onError, std::current_exception(), correlationID);
    }
}

} // namespace mbgl
