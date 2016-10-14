#include <mbgl/tile/raster_tile_worker.hpp>
#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/raster_bucket_parameters.hpp>
#include <mbgl/style/geometry_bucket_parameters.hpp>
#include <mbgl/actor/actor.hpp>

namespace mbgl {

using namespace style;

RasterTileWorker::RasterTileWorker(ActorRef<RasterTileWorker>,
                                   ActorRef<RasterTile> parent_,
                                   OverscaledTileID id_)
    : parent(std::move(parent_)), id(std::move(id_)) {
}

RasterTileWorker::~RasterTileWorker() = default;

void RasterTileWorker::setLayers(std::vector<std::unique_ptr<Layer>> layers_) {
    layers = std::move(layers_);
}

void RasterTileWorker::parse(std::shared_ptr<const std::string> data) {
    if (!layers) {
        return;
    }

    try {
        std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;

        BucketParameters parameters = RasterBucketParameters{id, data};

        for (auto& layer : *layers) {
            std::unique_ptr<Bucket> bucket = layer->baseImpl->createBucket(parameters);
            if (bucket && bucket->hasData()) {
                buckets.emplace(layer->baseImpl->bucketName(), std::move(bucket));
            }
        }

        parent.invoke(&RasterTile::onParsed, RasterTile::ParseResult {
            std::move(buckets)
        });
    } catch (...) {
        parent.invoke(&RasterTile::onError, std::current_exception());
    }
}

} // namespace mbgl
