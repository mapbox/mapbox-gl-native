#include <mbgl/tile/raster_tile_worker.hpp>
#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/renderer/raster_bucket.cpp>
#include <mbgl/actor/actor.hpp>
#include <mbgl/util/premultiply.hpp>

namespace mbgl {

RasterTileWorker::RasterTileWorker(ActorRef<RasterTileWorker>, ActorRef<RasterTile> parent_)
    : parent(std::move(parent_)) {
}

void RasterTileWorker::parse(std::shared_ptr<const std::string> data) {
    if (!data) {
        parent.invoke(&RasterTile::onParsed, nullptr); // No data; empty tile.
        return;
    }

    try {
        auto bucket = std::make_unique<RasterBucket>(util::unpremultiply(decodeImage(*data)));
        parent.invoke(&RasterTile::onParsed, std::move(bucket));
    } catch (...) {
        parent.invoke(&RasterTile::onError, std::current_exception());
    }
}

} // namespace mbgl
