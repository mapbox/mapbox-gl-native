#include <mbgl/tile/geometry_tile.hpp>

#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/renderer/render_source.hpp>
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
#include <mbgl/renderer/tile_render_data.hpp>

#include <mbgl/gfx/upload_pass.hpp>

namespace mbgl {

LayerRenderData* GeometryTile::LayoutResult::getLayerRenderData(const style::Layer::Impl& layerImpl) {
    auto it = layerRenderData.find(layerImpl.id);
    if (it == layerRenderData.end()) {
        return nullptr;
    }
    LayerRenderData& result = it->second;
    if (result.layerProperties->baseImpl->getTypeInfo() != layerImpl.getTypeInfo()) {
        // Layer data might be outdated, see issue #12432.
        return nullptr;
    }
    return &result;   
}

class GeometryTileRenderData final : public TileRenderData {
public:
    GeometryTileRenderData(
        std::shared_ptr<GeometryTile::LayoutResult> layoutResult_,
        std::shared_ptr<TileAtlasTextures> atlasTextures_)
        : TileRenderData(std::move(atlasTextures_))
        , layoutResult(std::move(layoutResult_)) {
    }

private:
    // TileRenderData overrides.
    optional<ImagePosition> getPattern(const std::string&) const override;
    const LayerRenderData* getLayerRenderData(const style::Layer::Impl&) const override;
    Bucket* getBucket(const style::Layer::Impl&) const override;
    void upload(gfx::UploadPass&) override;
    void prepare(const SourcePrepareParameters&) override;

    std::shared_ptr<GeometryTile::LayoutResult> layoutResult;
    std::vector<ImagePatch> imagePatches;
};

using namespace style;

optional<ImagePosition> GeometryTileRenderData::getPattern(const std::string& pattern) const {
    if (layoutResult) {
        const auto& iconAtlas = layoutResult->iconAtlas;
        auto it = iconAtlas.patternPositions.find(pattern);
        if (it != iconAtlas.patternPositions.end()) {
            return it->second;
        }
    }
    return nullopt;
}

void GeometryTileRenderData::upload(gfx::UploadPass& uploadPass) {
    if (!layoutResult) return;

    auto uploadFn = [&] (Bucket& bucket) {
        if (bucket.needsUpload()) {
            bucket.upload(uploadPass);
        }
    };

    for (auto& entry : layoutResult->layerRenderData) {
        uploadFn(*entry.second.bucket);
    }

    assert(atlasTextures);

    if (layoutResult->glyphAtlasImage) {
        atlasTextures->glyph = uploadPass.createTexture(*layoutResult->glyphAtlasImage);
        layoutResult->glyphAtlasImage = {};
    }

    if (layoutResult->iconAtlas.image.valid()) {
        atlasTextures->icon = uploadPass.createTexture(layoutResult->iconAtlas.image);
        layoutResult->iconAtlas.image = {};
    }

    if (atlasTextures->icon && !imagePatches.empty()) {
        for (const auto& imagePatch : imagePatches) { // patch updated images.
            uploadPass.updateTextureSub(*atlasTextures->icon, imagePatch.image->image, imagePatch.textureRect.x, imagePatch.textureRect.y);
        }
        imagePatches.clear();
    }
}

void GeometryTileRenderData::prepare(const SourcePrepareParameters& parameters) {
    if (!layoutResult) return;
    imagePatches = layoutResult->iconAtlas.getImagePatchesAndUpdateVersions(parameters.imageManager);
}

Bucket* GeometryTileRenderData::getBucket(const Layer::Impl& layer) const {
    const LayerRenderData* data = getLayerRenderData(layer);
    return data ? data->bucket.get() : nullptr; 
}

const LayerRenderData* GeometryTileRenderData::getLayerRenderData(const style::Layer::Impl& layerImpl) const {
    return layoutResult ? layoutResult->getLayerRenderData(layerImpl) : nullptr;
}


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
      fileSource(parameters.fileSource),
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
    worker.self().invoke(
        &GeometryTileWorker::setData, std::move(data_), imageManager.getAvailableImages(), correlationID);
}

void GeometryTile::reset() {
    // Mark the tile as pending again if it was complete before to prevent signaling a complete
    // state despite pending parse operations.
    pending = true;

    ++correlationID;
    worker.self().invoke(&GeometryTileWorker::reset, correlationID);
}

std::unique_ptr<TileRenderData> GeometryTile::createRenderData() {
    return std::make_unique<GeometryTileRenderData>(layoutResult, atlasTextures);
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
    worker.self().invoke(
        &GeometryTileWorker::setLayers, std::move(impls), imageManager.getAvailableImages(), correlationID);
}

void GeometryTile::setShowCollisionBoxes(const bool showCollisionBoxes_) {
    if (showCollisionBoxes != showCollisionBoxes_) {
        showCollisionBoxes = showCollisionBoxes_;
        ++correlationID;
        worker.self().invoke(&GeometryTileWorker::setShowCollisionBoxes, showCollisionBoxes, correlationID);
    }
}

void GeometryTile::onLayout(std::shared_ptr<LayoutResult> result, const uint64_t resultCorrelationID) {
    loaded = true;
    renderable = true;
    if (resultCorrelationID == correlationID) {
        pending = false;
    }

    layoutResult = std::move(result);
    if (!atlasTextures) {
    	atlasTextures = std::make_shared<TileAtlasTextures>();
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
    glyphManager.getGlyphs(*this, std::move(glyphDependencies), *fileSource);
}

void GeometryTile::onImagesAvailable(ImageMap images, ImageMap patterns, ImageVersionMap versionMap, uint64_t imageCorrelationID) {
    worker.self().invoke(&GeometryTileWorker::onImagesAvailable, std::move(images), std::move(patterns), std::move(versionMap), imageCorrelationID);
}

void GeometryTile::getImages(ImageRequestPair pair) {
    imageManager.getImages(*this, std::move(pair));
}

const std::shared_ptr<FeatureIndex> GeometryTile::getFeatureIndex() const {
    return layoutResult ? layoutResult->featureIndex : nullptr;
}

bool GeometryTile::layerPropertiesUpdated(const Immutable<style::LayerProperties>& layerProperties) {
    LayerRenderData* renderData = getLayerRenderData(*layerProperties->baseImpl);
    if (!renderData) {
        return false;
    }

    if (renderData->layerProperties != layerProperties &&
        renderData->layerProperties->constantsMask() == layerProperties->constantsMask()) {
        renderData->layerProperties = layerProperties;
    }

    return true;
}

const GeometryTileData* GeometryTile::getData() const {
    if (!layoutResult || !layoutResult->featureIndex) {
       return nullptr; 
    }
    return layoutResult->featureIndex->getData();
}

LayerRenderData* GeometryTile::getLayerRenderData(const style::Layer::Impl& layerImpl) {
    return layoutResult ? layoutResult->getLayerRenderData(layerImpl) : nullptr;
}

float GeometryTile::getQueryPadding(const std::unordered_map<std::string, const RenderLayer*>& layers) {
    float queryPadding = 0;
    for (const auto& pair : layers) {
        const LayerRenderData* data = getLayerRenderData(*pair.second->baseImpl);
        if (data && data->bucket && data->bucket->hasData()) {
            queryPadding = std::max(queryPadding, data->bucket->getQueryRadius(*pair.second));
        }
    }
    return queryPadding;
}

void GeometryTile::queryRenderedFeatures(std::unordered_map<std::string, std::vector<Feature>>& result,
                                         const GeometryCoordinates& queryGeometry, const TransformState& transformState,
                                         const std::unordered_map<std::string, const RenderLayer*>& layers,
                                         const RenderedQueryOptions& options, const mat4& projMatrix,
                                         const SourceFeatureState& featureState) {
    if (!getData()) return;

    const float queryPadding = getQueryPadding(layers);

    mat4 posMatrix;
    transformState.matrixFor(posMatrix, id.toUnwrapped());
    matrix::multiply(posMatrix, projMatrix, posMatrix);

    layoutResult->featureIndex->query(result, queryGeometry, transformState, posMatrix,
                                      util::tileSize * id.overscaleFactor(),
                                      std::pow(2, transformState.getZoom() - id.overscaledZ), options, id.toUnwrapped(),
                                      layers, queryPadding * transformState.maxPitchScaleFactor(), featureState);
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

                result.emplace_back(convertFeature(*feature, id.canonical));
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

void GeometryTile::setFeatureState(const LayerFeatureStates& states) {
    auto layers = getData();
    if ((layers == nullptr) || states.empty() || !layoutResult) {
        return;
    }

    auto& layerIdToLayerRenderData = layoutResult->layerRenderData;
    for (auto& layer : layerIdToLayerRenderData) {
        const auto& layerID = layer.first;
        const auto sourceLayer = layers->getLayer(layerID);
        if (sourceLayer) {
            const auto& sourceLayerID = sourceLayer->getName();
            auto entry = states.find(sourceLayerID);
            if (entry == states.end()) {
                continue;
            }
            const auto& featureStates = entry->second;
            if (featureStates.empty()) {
                continue;
            }

            auto bucket = layer.second.bucket;
            if (bucket && bucket->hasData()) {
                bucket->update(featureStates, *sourceLayer, layerID, layoutResult->iconAtlas.patternPositions);
            }
        }
    }
}

} // namespace mbgl
