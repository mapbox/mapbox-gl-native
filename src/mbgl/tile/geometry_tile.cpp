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
#include <mbgl/map/transform_state.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/actor/scheduler.hpp>

#include <iostream>

namespace mbgl {

using namespace style;

/*
   Correlation between GeometryTile and GeometryTileWorker is safeguarded by two
   correlation schemes:

   GeometryTile's 'correlationID' is used for ensuring the tile will be flagged
   as non-pending only when the placement coming from the last operation (as in
   'setData', 'setLayers',  'setShowCollisionBoxes') occurs. This is important for
   still mode rendering as we want to render only when all layout and placement
   operations are completed.

   GeometryTileWorker's 'imageCorrelationID' is used for checking whether an
   image request reply coming from `GeometryTile` is valid. Previous image
   request replies are ignored as they result in incomplete placement attempts
   that could flag the tile as non-pending too early.
 */

GeometryTile::GeometryTile(const OverscaledTileID& id_,
                           std::string sourceID_,
                           const TileParameters& parameters)
    : Tile(id_),
      sourceID(std::move(sourceID_)),
      mailbox(std::make_shared<Mailbox>(*Scheduler::GetCurrent())),
      worker(parameters.workerScheduler,
             ActorRef<GeometryTile>(*this, mailbox),
             id_,
             sourceID,
             obsolete,
             parameters.mode,
             parameters.pixelRatio,
             parameters.debugOptions & MapDebugOptions::Collision),
      glyphManager(parameters.glyphManager),
      imageManager(parameters.imageManager),
      mode(parameters.mode),
      showCollisionBoxes(parameters.debugOptions & MapDebugOptions::Collision) {
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
    observer->onTileError(*this, err);
}

void GeometryTile::setData(std::unique_ptr<const GeometryTileData> data_) {
    // Mark the tile as pending again if it was complete before to prevent signaling a complete
    // state despite pending parse operations.
    pending = true;

    ++correlationID;
    worker.invoke(&GeometryTileWorker::setData, std::move(data_), correlationID);
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

void GeometryTile::setShowCollisionBoxes(const bool showCollisionBoxes_) {
    if (showCollisionBoxes != showCollisionBoxes_) {
        showCollisionBoxes = showCollisionBoxes_;
        ++correlationID;
        worker.invoke(&GeometryTileWorker::setShowCollisionBoxes, showCollisionBoxes, correlationID);
    }
}

void GeometryTile::onLayout(LayoutResult result, const uint64_t resultCorrelationID) {
    // Don't mark ourselves loaded or renderable until the first successful placement
    // TODO: Ideally we'd render this tile without symbols as long as this tile wasn't
    //  replacing a tile at a different zoom that _did_ have symbols.
    (void)resultCorrelationID;
    nonSymbolBuckets = std::move(result.nonSymbolBuckets);
    pendingFeatureIndex = std::move(result.featureIndex);
    pendingData = std::move(result.tileData);
    observer->onTileChanged(*this);
}

void GeometryTile::onPlacement(PlacementResult result, const uint64_t resultCorrelationID) {
    loaded = true;
    renderable = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }
    symbolBuckets = std::move(result.symbolBuckets);
    if (result.glyphAtlasImage) {
        glyphAtlasImage = std::move(*result.glyphAtlasImage);
    }
    if (result.iconAtlasImage) {
        iconAtlasImage = std::move(*result.iconAtlasImage);
    }

    observer->onTileChanged(*this);
}

void GeometryTile::onError(std::exception_ptr err, const uint64_t resultCorrelationID) {
    loaded = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }
    observer->onTileError(*this, err);
}
    
void GeometryTile::onGlyphsAvailable(GlyphMap glyphs) {
    worker.invoke(&GeometryTileWorker::onGlyphsAvailable, std::move(glyphs));
}

void GeometryTile::getGlyphs(GlyphDependencies glyphDependencies) {
    glyphManager.getGlyphs(*this, std::move(glyphDependencies));
}

void GeometryTile::onImagesAvailable(ImageMap images, uint64_t imageCorrelationID) {
    worker.invoke(&GeometryTileWorker::onImagesAvailable, std::move(images), imageCorrelationID);
}

void GeometryTile::getImages(ImageRequestPair pair) {
    imageManager.getImages(*this, std::move(pair));
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

void GeometryTile::commitFeatureIndex() {
    if (pendingFeatureIndex) {
        featureIndex = std::move(pendingFeatureIndex);
    }
    if (pendingData) {
        data = std::move(pendingData);
    }
}

void GeometryTile::queryRenderedFeatures(
    std::unordered_map<std::string, std::vector<Feature>>& result,
    const GeometryCoordinates& queryGeometry,
    const TransformState& transformState,
    const std::vector<const RenderLayer*>& layers,
    const RenderedQueryOptions& options,
    const CollisionIndex& collisionIndex) {

    if (!featureIndex || !data) return;

    // Determine the additional radius needed factoring in property functions
    float additionalRadius = 0;
    for (const RenderLayer* layer : layers) {
        auto bucket = getBucket(*layer->baseImpl);
        if (bucket) {
            additionalRadius = std::max(additionalRadius, bucket->getQueryRadius(*layer));
        }
    }

    featureIndex->query(result,
                        queryGeometry,
                        transformState.getAngle(),
                        util::tileSize * id.overscaleFactor(),
                        std::pow(2, transformState.getZoom() - id.overscaledZ),
                        options,
                        *data,
                        id.toUnwrapped(),
                        sourceID,
                        layers,
                        collisionIndex,
                        additionalRadius);
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

bool GeometryTile::holdForFade() const {
    return mode == MapMode::Continuous &&
           (fadeState == FadeState::NeedsFirstPlacement || fadeState == FadeState::NeedsSecondPlacement);
}

void GeometryTile::markRenderedIdeal() {
    fadeState = FadeState::Loaded;
}
void GeometryTile::markRenderedPreviously() {
    if (fadeState == FadeState::Loaded) {
        fadeState = FadeState::NeedsFirstPlacement;
    }
}
void GeometryTile::performedFadePlacement() {
    if (fadeState == FadeState::NeedsFirstPlacement) {
        fadeState = FadeState::NeedsSecondPlacement;
    } else if (fadeState == FadeState::NeedsSecondPlacement) {
        fadeState = FadeState::CanRemove;
    }
}

} // namespace mbgl
