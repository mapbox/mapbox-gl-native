#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/render_background_layer.hpp>
#include <mbgl/renderer/render_custom_layer.hpp>
#include <mbgl/renderer/render_symbol_layer.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/query.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/logging.hpp>

#include <iostream>

namespace mbgl {

using namespace style;

GeometryTile::GeometryTile(const OverscaledTileID& id_,
                           std::string sourceID_,
                           const TileParameters& parameters,
                           GlyphAtlas& glyphAtlas_,
                           SpriteAtlas& spriteAtlas_)
    : Tile(id_),
      sourceID(std::move(sourceID_)),
      style(parameters.style),
      mailbox(std::make_shared<Mailbox>(*util::RunLoop::Get())),
      worker(parameters.workerScheduler,
             ActorRef<GeometryTile>(*this, mailbox),
             id_,
             obsolete,
             parameters.mode),
      glyphAtlas(glyphAtlas_),
      spriteAtlas(spriteAtlas_),
      placementThrottler(Milliseconds(300), [this] { invokePlacement(); }) {
}

GeometryTile::~GeometryTile() {
    glyphAtlas.removeGlyphs(*this);
    spriteAtlas.removeRequestor(*this);
    cancel();
}

void GeometryTile::cancel() {
    obsolete = true;
}

void GeometryTile::setError(std::exception_ptr err) {
    loaded = true;
    observer->onTileError(*this, err);
}

void GeometryTile::setData(std::unique_ptr<const GeometryTileData> data_) {
    // Mark the tile as pending again if it was complete before to prevent signaling a complete
    // state despite pending parse operations.
    pending = true;

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
    pending = true;

    ++correlationID;
    requestedConfig = desiredConfig;
    placementThrottler.invoke();
}

void GeometryTile::invokePlacement() {
    if (requestedConfig) {
        worker.invoke(&GeometryTileWorker::setPlacementConfig, *requestedConfig, correlationID);
    }
}

void GeometryTile::redoLayout() {
    // Mark the tile as pending again if it was complete before to prevent signaling a complete
    // state despite pending parse operations.
    pending = true;

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

void GeometryTile::onLayout(LayoutResult result, const uint64_t resultCorrelationID) {
    loaded = true;
    renderable = true;
    (void)resultCorrelationID;
    nonSymbolBuckets = std::move(result.nonSymbolBuckets);
    featureIndex = std::move(result.featureIndex);
    data = std::move(result.tileData);
    collisionTile.reset();
    observer->onTileChanged(*this);
}

void GeometryTile::onPlacement(PlacementResult result, const uint64_t resultCorrelationID) {
    loaded = true;
    renderable = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }
    symbolBuckets = std::move(result.symbolBuckets);
    for (auto& entry : symbolBuckets) {
        dynamic_cast<SymbolBucket*>(entry.second.get())->spriteAtlas = &spriteAtlas;
    }
    collisionTile = std::move(result.collisionTile);
    observer->onTileChanged(*this);
}

void GeometryTile::onError(std::exception_ptr err, const uint64_t resultCorrelationID) {
    loaded = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }
    observer->onTileError(*this, err);
}
    
void GeometryTile::onGlyphsAvailable(GlyphPositionMap glyphPositions) {
    worker.invoke(&GeometryTileWorker::onGlyphsAvailable, std::move(glyphPositions));
}

void GeometryTile::getGlyphs(GlyphDependencies glyphDependencies) {
    glyphAtlas.getGlyphs(*this, std::move(glyphDependencies));
}

void GeometryTile::onIconsAvailable(IconMap icons) {
    worker.invoke(&GeometryTileWorker::onIconsAvailable, std::move(icons));
}

void GeometryTile::getIcons(IconDependencies) {
    spriteAtlas.getIcons(*this);
}

Bucket* GeometryTile::getBucket(const RenderLayer& layer) const {
    const auto& buckets = layer.is<RenderSymbolLayer>() ? symbolBuckets : nonSymbolBuckets;
    const auto it = buckets.find(layer.baseImpl.id);
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
    const RenderedQueryOptions& options) {

    if (!featureIndex || !data) return;

    featureIndex->query(result,
                        queryGeometry,
                        transformState.getAngle(),
                        util::tileSize * id.overscaleFactor(),
                        std::pow(2, transformState.getZoom() - id.overscaledZ),
                        options,
                        *data,
                        id.canonical,
                        style,
                        collisionTile.get(),
                        *this);
}

void GeometryTile::querySourceFeatures(
    std::vector<Feature>& result,
    const SourceQueryOptions& options) {

    // Data not yet available
    if (!data) {
        return;
    }
    
    // No source layers, specified, nothing to do
    if (!options.sourceLayers) {
        Log::Warning(Event::General, "At least one sourceLayer required");
        return;
    }

    for (auto sourceLayer : *options.sourceLayers) {
        // Go throught all sourceLayers, if any
        // to gather all the features
        auto layer = data->getLayer(sourceLayer);
        
        if (layer) {
            auto featureCount = layer->featureCount();
            for (std::size_t i = 0; i < featureCount; i++) {
                auto feature = layer->getFeature(i);

                // Apply filter, if any
                if (options.filter && !(*options.filter)(*feature)) {
                    continue;
                }

                result.push_back(convertFeature(*feature, id.canonical));
            }
        }
    }
}

} // namespace mbgl
