#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {

using namespace style;

GeometryTile::GeometryTile(const OverscaledTileID& id_,
                           std::string sourceID_,
                           Style& style_,
                           const MapMode mode_)
    : Tile(id_),
      sourceID(std::move(sourceID_)),
      style(style_),
      worker(style_.workers),
      tileWorker(id_,
                 *style_.spriteStore,
                 *style_.glyphAtlas,
                 *style_.glyphStore,
                 obsolete,
                 mode_) {
}

GeometryTile::~GeometryTile() {
    cancel();
}

void GeometryTile::setError(std::exception_ptr err) {
    observer->onTileError(*this, err);
}

std::vector<std::unique_ptr<Layer>> GeometryTile::cloneStyleLayers() const {
    std::vector<std::unique_ptr<Layer>> result;

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

        result.push_back(layer->baseImpl->clone());
    }

    return result;
}

void GeometryTile::setData(std::unique_ptr<const GeometryTileData> data_) {
    if (!data_) {
        // This is a 404 response. We're treating these as empty tiles.
        workRequest.reset();
        availableData = DataAvailability::All;
        buckets.clear();
        redoPlacement();
        observer->onTileLoaded(*this, true);
        return;
    }

    // Mark the tile as pending again if it was complete before to prevent signaling a complete
    // state despite pending parse operations.
    if (availableData == DataAvailability::All) {
        availableData = DataAvailability::Some;
    }

    // Kick off a fresh parse of this tile. This happens when the tile is new, or
    // when tile data changed. Replacing the workdRequest will cancel a pending work
    // request in case there is one.
    workRequest.reset();
    workRequest = worker.parseGeometryTile(tileWorker, cloneStyleLayers(), std::move(data_), targetConfig, [this, config = targetConfig] (TileParseResult result) {
        workRequest.reset();

        if (result.is<TileParseResultData>()) {
            auto& resultBuckets = result.get<TileParseResultData>();
            availableData = resultBuckets.complete ? DataAvailability::All : DataAvailability::Some;

            // Persist the configuration we just placed so that we can later check whether we need to
            // place again in case the configuration has changed.
            placedConfig = config;

            // Move over all buckets we received in this parse request, potentially overwriting
            // existing buckets in case we got a refresh parse.
            buckets = std::move(resultBuckets.buckets);

            if (isComplete()) {
                featureIndex = std::move(resultBuckets.featureIndex);
                data = std::move(resultBuckets.tileData);
            }

            redoPlacement();
            observer->onTileLoaded(*this, true);
        } else {
            availableData = DataAvailability::All;
            observer->onTileError(*this, result.get<std::exception_ptr>());
        }
    });
}

bool GeometryTile::parsePending() {
    if (workRequest) {
        // There's already parsing or placement going on.
        return false;
    }

    workRequest.reset();
    workRequest = worker.parsePendingGeometryTileLayers(tileWorker, targetConfig, [this, config = targetConfig] (TileParseResult result) {
        workRequest.reset();

        if (result.is<TileParseResultData>()) {
            auto& resultBuckets = result.get<TileParseResultData>();
            availableData = resultBuckets.complete ? DataAvailability::All : DataAvailability::Some;

            // Move over all buckets we received in this parse request, potentially overwriting
            // existing buckets in case we got a refresh parse.
            for (auto& bucket : resultBuckets.buckets) {
                buckets[bucket.first] = std::move(bucket.second);
            }

            // Persist the configuration we just placed so that we can later check whether we need to
            // place again in case the configuration has changed.
            placedConfig = config;

            if (isComplete()) {
                featureIndex = std::move(resultBuckets.featureIndex);
                data = std::move(resultBuckets.tileData);
            }

            redoPlacement();
            observer->onTileLoaded(*this, false);
        } else {
            availableData = DataAvailability::All;
            observer->onTileError(*this, result.get<std::exception_ptr>());
        }
    });

    return true;
}

Bucket* GeometryTile::getBucket(const Layer& layer) {
    const auto it = buckets.find(layer.baseImpl->bucketName());
    if (it == buckets.end()) {
        return nullptr;
    }

    assert(it->second);
    return it->second.get();
}

void GeometryTile::redoPlacement(const PlacementConfig newConfig) {
    targetConfig = newConfig;
    redoPlacement();
}

void GeometryTile::redoPlacement() {
    // Don't start a new placement request when the current one hasn't completed yet, or when
    // we are parsing buckets.
    if (workRequest || targetConfig == placedConfig) {
        return;
    }

    workRequest = worker.redoPlacement(tileWorker, buckets, targetConfig, [this, config = targetConfig](std::unique_ptr<CollisionTile> collisionTile) {
        workRequest.reset();

        // Persist the configuration we just placed so that we can later check whether we need to
        // place again in case the configuration has changed.
        placedConfig = config;

        for (auto& bucket : buckets) {
            bucket.second->swapRenderData();
        }

        if (featureIndex) {
            featureIndex->setCollisionTile(std::move(collisionTile));
        }

        // The target configuration could have changed since we started placement. In this case,
        // we're starting another placement run.
        if (placedConfig != targetConfig) {
            redoPlacement();
        } else {
            observer->onNeedsRepaint();
        }
    });
}

void GeometryTile::queryRenderedFeatures(
    std::unordered_map<std::string, std::vector<Feature>>& result,
    const GeometryCoordinates& queryGeometry,
    const TransformState& transformState,
    const optional<std::vector<std::string>>& layerIDs) {

    if (!featureIndex || !data) return;

    featureIndex->query(result,
                        { queryGeometry },
                        transformState.getAngle(),
                        util::tileSize * id.overscaleFactor(),
                        std::pow(2, transformState.getZoom() - id.overscaledZ),
                        layerIDs,
                        *data,
                        id.canonical,
                        style);
}

void GeometryTile::cancel() {
    obsolete = true;
    workRequest.reset();
}

} // namespace mbgl
