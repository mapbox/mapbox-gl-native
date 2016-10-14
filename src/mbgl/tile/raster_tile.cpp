#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/tile/raster_tile_worker.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/tile_loader_impl.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/terrain_layer.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/util/run_loop.hpp>

namespace mbgl {

using namespace style;

RasterTile::RasterTile(const OverscaledTileID& id_,
                       std::string sourceID_,
                       const style::UpdateParameters& parameters,
                       const Tileset& tileset)
    : Tile(id_),
      loader(*this, id_, parameters, tileset),
      sourceID(std::move(sourceID_)),
      style(parameters.style),
      mailbox(std::make_shared<Mailbox>(*util::RunLoop::Get())),
      worker(parameters.workerScheduler, ActorRef<RasterTile>(*this, mailbox), id_) {
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

    std::vector<std::unique_ptr<Layer>> copy;

    for (const Layer* layer : style.getLayers()) {
        // Avoid cloning and including irrelevant layers.
        if (!(layer->is<RasterLayer>() || layer->is<TerrainLayer>()) ||
            layer->baseImpl->source != sourceID ||
            id.overscaledZ < std::floor(layer->baseImpl->minZoom) ||
            id.overscaledZ >= std::ceil(layer->baseImpl->maxZoom) ||
            layer->baseImpl->visibility == VisibilityType::None) {
            continue;
        }

        copy.push_back(layer->baseImpl->clone());
    }
    worker.invoke(&RasterTileWorker::setLayers, std::move(copy));
    worker.invoke(&RasterTileWorker::parse, data);
}

void RasterTile::onParsed(ParseResult result) {
    availableData = DataAvailability::All;
    buckets = std::move(result.buckets);
    observer->onTileChanged(*this);
}

void RasterTile::onError(std::exception_ptr err) {
    availableData = DataAvailability::All;
    observer->onTileError(*this, err);
}

Bucket* RasterTile::getBucket(const style::Layer& layer) {
    const auto it = buckets.find(layer.baseImpl->bucketName());
    if (it == buckets.end()) {
        return nullptr;
    }

    assert(it->second);
    return it->second.get();
}

void RasterTile::setNecessity(Necessity necessity) {
    loader.setNecessity(necessity);
}

} // namespace mbgl
