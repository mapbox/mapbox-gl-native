#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/run_loop.hpp>

namespace mbgl {

using namespace style;

GeometryTile::GeometryTile(const OverscaledTileID& id_,
                           std::string sourceID_,
                           const style::UpdateParameters& parameters)
    : Tile(id_),
      sourceID(std::move(sourceID_)),
      style(parameters.style),
      mailbox(std::make_shared<Mailbox>(*util::RunLoop::Get())),
      worker(parameters.workerScheduler,
             ActorRef<GeometryTile>(*this, mailbox),
             id_,
             *parameters.style.glyphAtlas,
             obsolete,
             parameters.mode) {
}

GeometryTile::~GeometryTile() {
    cancel();
}

void GeometryTile::cancel() {
    obsolete = true;
}

void GeometryTile::setError(std::exception_ptr err) {
    observer->onTileError(*this, err);
}

void GeometryTile::setData(std::unique_ptr<const GeometryTileData> data_) {
    // Mark the tile as pending again if it was complete before to prevent signaling a complete
    // state despite pending parse operations.
    if (availableData == DataAvailability::All) {
        availableData = DataAvailability::Some;
    }

    ++correlationID;
    worker.invoke(&GeometryTileWorker::setData, std::move(data_), correlationID);
    redoLayout();
}

void GeometryTile::setPlacementConfig(const PlacementConfig& desiredConfig) {
    if (requestedConfig == desiredConfig) {
        return;
    }

    // Mark the tile as pending again if it was complete before to prevent signaling a complete
    // state despite pending parse operations.
    if (availableData == DataAvailability::All) {
        availableData = DataAvailability::Some;
    }

    ++correlationID;
    requestedConfig = desiredConfig;
    worker.invoke(&GeometryTileWorker::setPlacementConfig, desiredConfig, correlationID);
}

void GeometryTile::symbolDependenciesChanged() {
    worker.invoke(&GeometryTileWorker::symbolDependenciesChanged);
}

void GeometryTile::redoLayout() {
    // Mark the tile as pending again if it was complete before to prevent signaling a complete
    // state despite pending parse operations.
    if (availableData == DataAvailability::All) {
        availableData = DataAvailability::Some;
    }

    std::vector<std::unique_ptr<Layer>> copy;

    for (const Layer* layer : style.getLayers()) {
        // Avoid cloning and including irrelevant layers.
        if (layer->is<BackgroundLayer>() ||
            layer->is<CustomLayer>() ||
            layer->baseImpl->source != sourceID ||
            id.overscaledZ < std::floor(layer->baseImpl->minZoom) ||
            id.overscaledZ >= std::ceil(layer->baseImpl->maxZoom) ||
            layer->baseImpl->visibility == VisibilityType::None) {
            continue;
        }

        copy.push_back(layer->baseImpl->clone());
    }

    ++correlationID;
    worker.invoke(&GeometryTileWorker::setLayers, std::move(copy), correlationID);
}

void GeometryTile::onLayout(LayoutResult result) {
    availableData = DataAvailability::Some;
    nonSymbolBuckets = std::move(result.nonSymbolBuckets);
    featureIndex = std::move(result.featureIndex);
    data = std::move(result.tileData);
    observer->onTileChanged(*this);
}

void GeometryTile::onPlacement(PlacementResult result) {
    if (result.correlationID == correlationID) {
        availableData = DataAvailability::All;
    }
    symbolBuckets = std::move(result.symbolBuckets);
    collisionTile = std::move(result.collisionTile);
    observer->onTileChanged(*this);
}

void GeometryTile::onError(std::exception_ptr err) {
    availableData = DataAvailability::All;
    observer->onTileError(*this, err);
}

Bucket* GeometryTile::getBucket(const Layer& layer) {
    const auto& buckets = layer.is<SymbolLayer>() ? symbolBuckets : nonSymbolBuckets;
    const auto it = buckets.find(layer.baseImpl->id);
    if (it == buckets.end()) {
        return nullptr;
    }

    assert(it->second);
    return it->second.get();
}

void GeometryTile::queryRenderedFeatures(
    std::unordered_map<std::string, std::vector<Feature>>& result,
    const GeometryCoordinates& queryGeometry,
    const TransformState& transformState,
    const optional<std::vector<std::string>>& layerIDs) {

    if (!featureIndex || !data) return;

    featureIndex->query(result,
                        queryGeometry,
                        transformState.getAngle(),
                        util::tileSize * id.overscaleFactor(),
                        std::pow(2, transformState.getZoom() - id.overscaledZ),
                        layerIDs,
                        *data,
                        id.canonical,
                        style,
                        collisionTile.get());
}

} // namespace mbgl
