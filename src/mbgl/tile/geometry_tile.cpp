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
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/actor/scheduler.hpp>

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
    : Tile(Kind::Geometry, id_),
      ImageRequestor(parameters.imageManager),
      sourceID(std::move(sourceID_)),
      mailbox(std::make_shared<Mailbox>(*Scheduler::GetCurrent())),
      worker(Scheduler::GetBackground(),
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
    worker.self().invoke(&GeometryTileWorker::setData, std::move(data_), correlationID);
}


void GeometryTile::setLayers(const std::vector<Immutable<LayerProperties>>& layers) {
    // Mark the tile as pending again if it was complete before to prevent signaling a complete
    // state despite pending parse operations.
    pending = true;

    std::vector<Immutable<LayerProperties>> impls;
    impls.reserve(layers.size());

    for (const auto& layer : layers) {
        // Skip irrelevant layers.
        const auto& layerImpl = *layer->baseImpl;
        assert(layerImpl.getTypeInfo()->source != LayerTypeInfo::Source::NotRequired);
        assert(layerImpl.source == sourceID);
        assert(layerImpl.visibility != VisibilityType::None);
        if (id.overscaledZ < std::floor(layerImpl.minZoom) ||
            id.overscaledZ >= std::ceil(layerImpl.maxZoom)) {
            continue;
        }

        impls.push_back(layer);
    }

    ++correlationID;
    worker.self().invoke(&GeometryTileWorker::setLayers, std::move(impls), correlationID);
}

void GeometryTile::setShowCollisionBoxes(const bool showCollisionBoxes_) {
    if (showCollisionBoxes != showCollisionBoxes_) {
        showCollisionBoxes = showCollisionBoxes_;
        ++correlationID;
        worker.self().invoke(&GeometryTileWorker::setShowCollisionBoxes, showCollisionBoxes, correlationID);
    }
}

void GeometryTile::onLayout(LayoutResult result, const uint64_t resultCorrelationID) {
    loaded = true;
    renderable = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }
    
    layerIdToLayerRenderData = std::move(result.renderData);
    
    latestFeatureIndex = std::move(result.featureIndex);

    if (result.glyphAtlasImage) {
        glyphAtlasImage = std::move(*result.glyphAtlasImage);
    }
    if (result.iconAtlas.image.valid()) {
        iconAtlas = std::move(result.iconAtlas);
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
    worker.self().invoke(&GeometryTileWorker::onGlyphsAvailable, std::move(glyphs));
}

void GeometryTile::getGlyphs(GlyphDependencies glyphDependencies) {
    glyphManager.getGlyphs(*this, std::move(glyphDependencies));
}

void GeometryTile::onImagesAvailable(ImageMap images, ImageMap patterns, ImageVersionMap versionMap, uint64_t imageCorrelationID) {
    worker.self().invoke(&GeometryTileWorker::onImagesAvailable, std::move(images), std::move(patterns), std::move(versionMap), imageCorrelationID);
}

void GeometryTile::getImages(ImageRequestPair pair) {
    imageManager.getImages(*this, std::move(pair));
}

const optional<ImagePosition> GeometryTile::getPattern(const std::string& pattern) {
    auto it = iconAtlas.patternPositions.find(pattern);
    if (it !=  iconAtlas.patternPositions.end()) {
        return it->second;
    }
    return {};
}

void GeometryTile::upload(gfx::Context& context) {
    auto uploadFn = [&] (Bucket& bucket) {
        if (bucket.needsUpload()) {
            bucket.upload(context);
        }
    };

    for (auto& entry : layerIdToLayerRenderData) {
        uploadFn(*entry.second.bucket);
    }

    if (glyphAtlasImage) {
        glyphAtlasTexture = context.createTexture(*glyphAtlasImage);
        glyphAtlasImage = {};
    }

    if (iconAtlas.image.valid()) {
        iconAtlasTexture = context.createTexture(iconAtlas.image);
        iconAtlas.image = {};
    }

    if (iconAtlasTexture) {
        iconAtlas.patchUpdatedImages(context, *iconAtlasTexture, imageManager);
    }
}

Bucket* GeometryTile::getBucket(const Layer::Impl& layer) const {
    const LayerRenderData* data = getLayerRenderData(layer);
    return data ? data->bucket.get() : nullptr; 
}

const LayerRenderData* GeometryTile::getLayerRenderData(const style::Layer::Impl& layerImpl) const {
    auto* that = const_cast<GeometryTile*>(this);
    return that->getMutableLayerRenderData(layerImpl);
}

bool GeometryTile::updateLayerProperties(const Immutable<style::LayerProperties>& layerProperties) {
    LayerRenderData* renderData = getMutableLayerRenderData(*layerProperties->baseImpl);
    if (!renderData) {
        return false;
    }

    if (renderData->layerProperties != layerProperties) {
        renderData->layerProperties = layerProperties;
    }

    return true;
}

LayerRenderData* GeometryTile::getMutableLayerRenderData(const style::Layer::Impl& layerImpl) {
    auto it = layerIdToLayerRenderData.find(layerImpl.id);
    if (it == layerIdToLayerRenderData.end()) {
        return nullptr;
    }
    LayerRenderData& result = it->second;
    if (result.layerProperties->baseImpl->getTypeInfo() != layerImpl.getTypeInfo()) {
        // Layer data might be outdated, see issue #12432.
        return nullptr;
    }
    return &result;   
}

float GeometryTile::getQueryPadding(const std::vector<const RenderLayer*>& layers) {
    float queryPadding = 0;
    for (const RenderLayer* layer : layers) {
        auto bucket = getBucket(*layer->baseImpl);
        if (bucket && bucket->hasData()) {
            queryPadding = std::max(queryPadding, bucket->getQueryRadius(*layer));
        }
    }
    return queryPadding;
}

void GeometryTile::queryRenderedFeatures(
    std::unordered_map<std::string, std::vector<Feature>>& result,
    const GeometryCoordinates& queryGeometry,
    const TransformState& transformState,
    const std::vector<const RenderLayer*>& layers,
    const RenderedQueryOptions& options,
    const mat4& projMatrix) {

    if (!getData()) return;

    const float queryPadding = getQueryPadding(layers);

    mat4 posMatrix;
    transformState.matrixFor(posMatrix, id.toUnwrapped());
    matrix::multiply(posMatrix, projMatrix, posMatrix);

    latestFeatureIndex->query(result,
                              queryGeometry,
                              transformState,
                              posMatrix,
                              util::tileSize * id.overscaleFactor(),
                              std::pow(2, transformState.getZoom() - id.overscaledZ),
                              options,
                              id.toUnwrapped(),
                              layers,
                              queryPadding * transformState.maxPitchScaleFactor());
}

void GeometryTile::querySourceFeatures(
    std::vector<Feature>& result,
    const SourceQueryOptions& options) {

    // Data not yet available, or tile is empty
    if (!getData()) {
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
        auto layer = getData()->getLayer(sourceLayer);
        
        if (layer) {
            auto featureCount = layer->featureCount();
            for (std::size_t i = 0; i < featureCount; i++) {
                auto feature = layer->getFeature(i);

                // Apply filter, if any
                if (options.filter && !(*options.filter)(style::expression::EvaluationContext { static_cast<float>(this->id.overscaledZ), feature.get() })) {
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
