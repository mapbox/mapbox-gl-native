#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/actor/scheduler.hpp>

#include <iostream>

namespace mbgl {

using namespace style;

GeometryTile::GeometryTile(const OverscaledTileID& id_,
                           std::string sourceID_,
                           const TileParameters& parameters)
    : Tile(id_),
      sourceID(std::move(sourceID_)),
      mailbox(std::make_shared<Mailbox>(*Scheduler::GetCurrent())),
      worker(parameters.workerScheduler,
             ActorRef<GeometryTile>(*this, mailbox),
             id_,
             obsolete,
             parameters.mode,
             parameters.pixelRatio),
      glyphManager(parameters.glyphManager),
      imageManager(parameters.imageManager),
      placementThrottler(Milliseconds(300), [this] { invokePlacement(); }),
      lastYStretch(1.0f) {
}

GeometryTile::~GeometryTile() {
    glyphManager.removeRequestor(*this);
    imageManager.removeRequestor(*this);
    markObsolete();
}

void GeometryTile::cancel() {
    markObsolete();
}

void GeometryTile::markObsolete() {
    obsolete = true;
}

void GeometryTile::setError(std::exception_ptr err) {
    loaded = true;
    renderable = false;
    observer->onTileError(*this, err);
}

void GeometryTile::setData(std::unique_ptr<const GeometryTileData> data_) {
    // Mark the tile as pending again if it was complete before to prevent signaling a complete
    // state despite pending parse operations.
    pending = true;

    ++correlationID;
    worker.invoke(&GeometryTileWorker::setData, std::move(data_), correlationID);
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

void GeometryTile::setLayers(const std::vector<Immutable<Layer::Impl>>& layers) {
    // Mark the tile as pending again if it was complete before to prevent signaling a complete
    // state despite pending parse operations.
    pending = true;

    std::vector<Immutable<Layer::Impl>> impls;

    for (const auto& layer : layers) {
        // Skip irrelevant layers.
        if (layer->type == LayerType::Background ||
            layer->type == LayerType::Custom ||
            layer->source != sourceID ||
            id.overscaledZ < std::floor(layer->minZoom) ||
            id.overscaledZ >= std::ceil(layer->maxZoom) ||
            layer->visibility == VisibilityType::None) {
            continue;
        }

        impls.push_back(layer);
    }

    ++correlationID;
    worker.invoke(&GeometryTileWorker::setLayers, std::move(impls), correlationID);
}

void GeometryTile::onLayout(LayoutResult result) {
    loaded = true;
    renderable = true;
    nonSymbolBuckets = std::move(result.nonSymbolBuckets);
    featureIndex = std::move(result.featureIndex);
    data = std::move(result.tileData);
    collisionTile.reset();
    observer->onTileChanged(*this);
}

void GeometryTile::onPlacement(PlacementResult result) {
    loaded = true;
    renderable = true;
    if (result.correlationID == correlationID) {
        pending = false;
    }
    symbolBuckets = std::move(result.symbolBuckets);
    collisionTile = std::move(result.collisionTile);
    if (result.glyphAtlasImage) {
        glyphAtlasImage = std::move(*result.glyphAtlasImage);
    }
    if (result.iconAtlasImage) {
        iconAtlasImage = std::move(*result.iconAtlasImage);
    }
    if (collisionTile.get()) {
        lastYStretch = collisionTile->yStretch;
    }
    observer->onTileChanged(*this);
}

void GeometryTile::onError(std::exception_ptr err) {
    loaded = true;
    pending = false;
    renderable = false;
    observer->onTileError(*this, err);
}
    
void GeometryTile::onGlyphsAvailable(GlyphMap glyphs) {
    worker.invoke(&GeometryTileWorker::onGlyphsAvailable, std::move(glyphs));
}

void GeometryTile::getGlyphs(GlyphDependencies glyphDependencies) {
    glyphManager.getGlyphs(*this, std::move(glyphDependencies));
}

void GeometryTile::onImagesAvailable(ImageMap images) {
    worker.invoke(&GeometryTileWorker::onImagesAvailable, std::move(images));
}

void GeometryTile::getImages(ImageDependencies imageDependencies) {
    imageManager.getImages(*this, std::move(imageDependencies));
}

void GeometryTile::upload(gl::Context& context) {
    auto uploadFn = [&] (Bucket& bucket) {
        if (bucket.needsUpload()) {
            bucket.upload(context);
        }
    };

    for (auto& entry : nonSymbolBuckets) {
        uploadFn(*entry.second);
    }

    for (auto& entry : symbolBuckets) {
        uploadFn(*entry.second);
    }

    if (glyphAtlasImage) {
        glyphAtlasTexture = context.createTexture(*glyphAtlasImage, 0);
        glyphAtlasImage = {};
    }

    if (iconAtlasImage) {
        iconAtlasTexture = context.createTexture(*iconAtlasImage, 0);
        iconAtlasImage = {};
    }
}

Bucket* GeometryTile::getBucket(const Layer::Impl& layer) const {
    const auto& buckets = layer.type == LayerType::Symbol ? symbolBuckets : nonSymbolBuckets;
    const auto it = buckets.find(layer.id);
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
    const std::vector<const RenderLayer*>& layers,
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
                        layers,
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

float GeometryTile::yStretch() const {
    // collisionTile gets reset in onLayout but we don't clear the symbolBuckets
    // until a new placement result comes along, so keep the yStretch value in
    // case we need to render them.
    return lastYStretch;
}

} // namespace mbgl
