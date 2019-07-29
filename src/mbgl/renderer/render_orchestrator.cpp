#include <mbgl/renderer/render_orchestrator.hpp>

#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/layermanager/layer_manager.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/renderer/render_tree.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/renderer/upload_parameters.hpp>
#include <mbgl/renderer/pattern_atlas.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/transition_parameters.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/style_diff.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

using namespace style;

static RendererObserver& nullObserver() {
    static RendererObserver observer;
    return observer;
}

namespace {

class LayerRenderItem final : public RenderItem {
public:
    LayerRenderItem(RenderLayer& layer_, RenderSource* source_, uint32_t index_)
        : layer(layer_), source(source_), index(index_) {}
    bool operator<(const LayerRenderItem& other) const { return index < other.index; }
    std::reference_wrapper<RenderLayer> layer;
    RenderSource* source;

private:
    bool hasRenderPass(RenderPass pass) const override { return layer.get().hasRenderPass(pass); }
    void upload(gfx::UploadPass& pass) const override { layer.get().upload(pass); }
    void render(PaintParameters& parameters) const override { layer.get().render(parameters); }
    const std::string& getName() const override { return layer.get().getID(); } 

    uint32_t index;
};

class RenderTreeImpl final : public RenderTree {
public:
    RenderTreeImpl(std::unique_ptr<RenderTreeParameters> parameters_,
                   std::set<LayerRenderItem> layerRenderItems_,
                   std::vector<std::unique_ptr<RenderItem>> sourceRenderItems_,
                   LineAtlas& lineAtlas_,
                   PatternAtlas& patternAtlas_)
        : RenderTree(std::move(parameters_)),
          layerRenderItems(std::move(layerRenderItems_)),
          sourceRenderItems(std::move(sourceRenderItems_)),
          lineAtlas(lineAtlas_),
          patternAtlas(patternAtlas_) {
    }

    RenderItems getLayerRenderItems() const override {
        return { layerRenderItems.begin(), layerRenderItems.end() };
    }
    RenderItems getSourceRenderItems() const override {
        RenderItems result;
        result.reserve(sourceRenderItems.size());
        for (const auto& item : sourceRenderItems) result.emplace_back(*item);
        return result;
    }
    LineAtlas& getLineAtlas() const override { return lineAtlas; }
    PatternAtlas& getPatternAtlas() const override { return patternAtlas; }

    std::set<LayerRenderItem> layerRenderItems;
    std::vector<std::unique_ptr<RenderItem>> sourceRenderItems;
    std::reference_wrapper<LineAtlas> lineAtlas;
    std::reference_wrapper<PatternAtlas> patternAtlas;
};

}  // namespace

RenderOrchestrator::RenderOrchestrator(
                     bool backgroundLayerAsColor_,
                     optional<std::string> localFontFamily_)
    : observer(&nullObserver())
    , glyphManager(std::make_unique<GlyphManager>(std::make_unique<LocalGlyphRasterizer>(std::move(localFontFamily_))))
    , imageManager(std::make_unique<ImageManager>())
    , lineAtlas(std::make_unique<LineAtlas>(Size{ 256, 512 }))
    , patternAtlas(std::make_unique<PatternAtlas>())
    , imageImpls(makeMutable<std::vector<Immutable<style::Image::Impl>>>())
    , sourceImpls(makeMutable<std::vector<Immutable<style::Source::Impl>>>())
    , layerImpls(makeMutable<std::vector<Immutable<style::Layer::Impl>>>())
    , renderLight(makeMutable<Light::Impl>())
    , placement(std::make_unique<Placement>(TransformState{}, MapMode::Static, TransitionOptions{}, true))
    , backgroundLayerAsColor(backgroundLayerAsColor_) {
    glyphManager->setObserver(this);
    imageManager->setObserver(this);
}

RenderOrchestrator::~RenderOrchestrator() {
    if (contextLost) {
        // Signal all RenderLayers that the context was lost
        // before cleaning up. At the moment, only CustomLayer is
        // interested whether rendering context is lost. However, it would be
        // beneficial for dynamically loaded or other custom built-in plugins.
        for (const auto& entry : renderLayers) {
            RenderLayer& layer = *entry.second;
            layer.markContextDestroyed();
        }
    }
};

void RenderOrchestrator::setObserver(RendererObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver();
}

std::unique_ptr<RenderTree> RenderOrchestrator::createRenderTree(const UpdateParameters& updateParameters) {
    const bool isMapModeContinuous = updateParameters.mode == MapMode::Continuous;
    if (!isMapModeContinuous) {
        // Reset zoom history state.
        zoomHistory.first = true;
    }

    if (LayerManager::annotationsEnabled) {
        updateParameters.annotationManager.updateData();
    }

    const bool zoomChanged = zoomHistory.update(updateParameters.transformState.getZoom(), updateParameters.timePoint);

    const TransitionOptions transitionOptions = isMapModeContinuous ? updateParameters.transitionOptions : TransitionOptions();

    const TransitionParameters transitionParameters {
        updateParameters.timePoint,
        transitionOptions
    };

    const PropertyEvaluationParameters evaluationParameters {
        zoomHistory,
        updateParameters.timePoint,
        transitionOptions.duration.value_or(isMapModeContinuous ? util::DEFAULT_TRANSITION_DURATION : Duration::zero())
    };

    const TileParameters tileParameters {
        updateParameters.pixelRatio,
        updateParameters.debugOptions,
        updateParameters.transformState,
        updateParameters.fileSource,
        updateParameters.mode,
        updateParameters.annotationManager,
        *imageManager,
        *glyphManager,
        updateParameters.prefetchZoomDelta
    };

    glyphManager->setURL(updateParameters.glyphURL);

    // Update light.
    const bool lightChanged = renderLight.impl != updateParameters.light;

    if (lightChanged) {
        renderLight.impl = updateParameters.light;
        renderLight.transition(transitionParameters);
    }

    if (lightChanged || zoomChanged || renderLight.hasTransition()) {
        renderLight.evaluate(evaluationParameters);
    }


    const ImageDifference imageDiff = diffImages(imageImpls, updateParameters.images);
    imageImpls = updateParameters.images;

    // Only trigger tile reparse for changed images. Changed images only need a relayout when they have a different size.
    bool hasImageDiff = !imageDiff.removed.empty();

    // Remove removed images from sprite atlas.
    for (const auto& entry : imageDiff.removed) {
        imageManager->removeImage(entry.first);
        patternAtlas->removePattern(entry.first);
    }

    // Add added images to sprite atlas.
    for (const auto& entry : imageDiff.added) {
        imageManager->addImage(entry.second);
    }

    // Update changed images.
    for (const auto& entry : imageDiff.changed) {
        if (imageManager->updateImage(entry.second.after)) {
            patternAtlas->removePattern(entry.first);
            hasImageDiff = true;
        }
    }

    imageManager->notifyIfMissingImageAdded();
    imageManager->setLoaded(updateParameters.spriteLoaded);

    const LayerDifference layerDiff = diffLayers(layerImpls, updateParameters.layers);
    layerImpls = updateParameters.layers;

    // Remove render layers for removed layers.
    for (const auto& entry : layerDiff.removed) {
        renderLayers.erase(entry.first);
    }

    // Create render layers for newly added layers.
    for (const auto& entry : layerDiff.added) {
        auto renderLayer = LayerManager::get()->createRenderLayer(entry.second);
        renderLayer->transition(transitionParameters);
        renderLayers.emplace(entry.first, std::move(renderLayer));
    }

    // Update render layers for changed layers.
    for (const auto& entry : layerDiff.changed) {
        renderLayers.at(entry.first)->transition(transitionParameters, entry.second.after);
    }

    if (!layerDiff.removed.empty() || !layerDiff.added.empty() || !layerDiff.changed.empty()) {
        glyphManager->evict(fontStacks(*updateParameters.layers));
    }

    // Update layers for class and zoom changes.
    std::unordered_set<std::string> constantsMaskChanged;
    for (const auto& entry : renderLayers) {
        RenderLayer& layer = *entry.second;
        const bool layerAddedOrChanged = layerDiff.added.count(entry.first) || layerDiff.changed.count(entry.first);
        if (layerAddedOrChanged || zoomChanged || layer.hasTransition() || layer.hasCrossfade()) {
            auto previousMask = layer.evaluatedProperties->constantsMask();
            layer.evaluate(evaluationParameters);
            if (previousMask != layer.evaluatedProperties->constantsMask()) {
                constantsMaskChanged.insert(layer.getID());
            }
        }
    }

    const SourceDifference sourceDiff = diffSources(sourceImpls, updateParameters.sources);
    sourceImpls = updateParameters.sources;

    // Remove render layers for removed sources.
    for (const auto& entry : sourceDiff.removed) {
        renderSources.erase(entry.first);
    }

    // Create render sources for newly added sources.
    for (const auto& entry : sourceDiff.added) {
        std::unique_ptr<RenderSource> renderSource = RenderSource::create(entry.second);
        renderSource->setObserver(this);
        renderSources.emplace(entry.first, std::move(renderSource));
    }
    transformState = updateParameters.transformState;

    // Create parameters for the render tree.
    auto renderTreeParameters = std::make_unique<RenderTreeParameters>(
        updateParameters.transformState,
        updateParameters.mode,
        updateParameters.debugOptions,
        updateParameters.timePoint,
        renderLight.getEvaluated());

    std::set<LayerRenderItem> layerRenderItems;
    std::vector<std::reference_wrapper<RenderLayer>> layersNeedPlacement;
    auto renderItemsEmplaceHint = layerRenderItems.begin();

    // Reserve size for filteredLayersForSource if there are sources.
    if (!sourceImpls->empty()) {
        filteredLayersForSource.reserve(layerImpls->size());
    }

    // Update all sources and initialize renderItems.
    for (const auto& sourceImpl : *sourceImpls) {
        RenderSource* source = renderSources.at(sourceImpl->id).get();
        bool sourceNeedsRendering = false;
        bool sourceNeedsRelayout = false;       
        
        uint32_t index = 0u;
        const auto begin = layerImpls->begin();
        const auto end = layerImpls->end();
        for (auto it = begin; it != end; ++it, ++index) {
            const Immutable<Layer::Impl>& layerImpl = *it;
            RenderLayer* layer = getRenderLayer(layerImpl->id);
            const auto* layerInfo = layerImpl->getTypeInfo();
            const bool layerIsVisible = layer->baseImpl->visibility != style::VisibilityType::None;
            const bool zoomFitsLayer = layer->supportsZoom(zoomHistory.lastZoom);
            renderTreeParameters->has3D |= (layerInfo->pass3d == LayerTypeInfo::Pass3D::Required);

            if (layerInfo->source != LayerTypeInfo::Source::NotRequired) {
                if (layerImpl->source == sourceImpl->id) {
                    sourceNeedsRelayout = (sourceNeedsRelayout || hasImageDiff || constantsMaskChanged.count(layerImpl->id) || hasLayoutDifference(layerDiff, layerImpl->id));
                    if (layerIsVisible) {
                        filteredLayersForSource.push_back(layer->evaluatedProperties);
                        if (zoomFitsLayer) {
                            sourceNeedsRendering = true;
                            renderItemsEmplaceHint = layerRenderItems.emplace_hint(renderItemsEmplaceHint, *layer, source, index);
                        }
                    }
                }
                continue;
            } 

            // Handle layers without source.
            if (layerIsVisible && zoomFitsLayer && sourceImpl.get() == sourceImpls->at(0).get()) {
                if (backgroundLayerAsColor && layerImpl.get() == layerImpls->at(0).get()) {
                    const auto& solidBackground = layer->getSolidBackground();
                    if (solidBackground) {
                        renderTreeParameters->backgroundColor = *solidBackground;
                        continue; // This layer is shown with background color, and it shall not be added to render items. 
                    }
                }
                renderItemsEmplaceHint = layerRenderItems.emplace_hint(renderItemsEmplaceHint, *layer, nullptr, index);
            }
        }
        source->update(sourceImpl,
                       filteredLayersForSource,
                       sourceNeedsRendering,
                       sourceNeedsRelayout,
                       tileParameters);
        filteredLayersForSource.clear();
    }

    renderTreeParameters->loaded = updateParameters.styleLoaded && isLoaded();
    if (!isMapModeContinuous && !renderTreeParameters->loaded) {
        return nullptr;
    }

    // Prepare. Update all matrices and generate data that we should upload to the GPU.
    for (const auto& entry : renderSources) {
        if (entry.second->isEnabled()) {
            entry.second->prepare({renderTreeParameters->transformParams, updateParameters.debugOptions, *imageManager});
        }
    }

    auto opaquePassCutOffEstimation = layerRenderItems.size();
    for (auto& renderItem : layerRenderItems) {
        RenderLayer& renderLayer = renderItem.layer;
        renderLayer.prepare({renderItem.source, *imageManager, *patternAtlas, *lineAtlas, updateParameters.transformState});
        if (renderLayer.needsPlacement()) {
            layersNeedPlacement.emplace_back(renderLayer);
        }
        if (renderTreeParameters->opaquePassCutOff == 0) {
            --opaquePassCutOffEstimation;
            if (renderLayer.is3D()) {
                renderTreeParameters->opaquePassCutOff = uint32_t(opaquePassCutOffEstimation);
            }
        }
    }
    // Symbol placement.
    {
        if (!isMapModeContinuous) {
            // TODO: Think about right way for symbol index to handle still rendering
            crossTileSymbolIndex.reset();
        }

        bool symbolBucketsChanged = false;
        const bool placementChanged = !placement->stillRecent(updateParameters.timePoint);
        std::set<std::string> usedSymbolLayers;
        if (placementChanged) {
            placement = std::make_unique<Placement>(
                updateParameters.transformState, updateParameters.mode,
                updateParameters.transitionOptions, updateParameters.crossSourceCollisions,
                std::move(placement));
        }

        for (auto it = layersNeedPlacement.rbegin(); it != layersNeedPlacement.rend(); ++it) {
            const RenderLayer& layer = *it;
            if (crossTileSymbolIndex.addLayer(layer, updateParameters.transformState.getLatLng().longitude())) symbolBucketsChanged = true;

            if (placementChanged) {
                usedSymbolLayers.insert(layer.getID());
                placement->placeLayer(layer, renderTreeParameters->transformParams.projMatrix, updateParameters.debugOptions & MapDebugOptions::Collision);
            }
        }

        if (placementChanged) {
            placement->commit(updateParameters.timePoint);
            crossTileSymbolIndex.pruneUnusedLayers(usedSymbolLayers);
            for (const auto& entry : renderSources) {
                entry.second->updateFadingTiles();
            }
        } else {
            placement->setStale();
        }

        for (auto it = layersNeedPlacement.rbegin(); it != layersNeedPlacement.rend(); ++it) {
            placement->updateLayerBuckets(*it, updateParameters.transformState, placementChanged || symbolBucketsChanged);
        }

        renderTreeParameters->symbolFadeChange = placement->symbolFadeChange(updateParameters.timePoint);
    }

    renderTreeParameters->needsRepaint = isMapModeContinuous && hasTransitions(updateParameters.timePoint);
    if (!renderTreeParameters->needsRepaint && renderTreeParameters->loaded) {
        // Notify observer about unused images when map is fully loaded
        // and there are no ongoing transitions.
        imageManager->reduceMemoryUseIfCacheSizeExceedsLimit();
    }

    std::vector<std::unique_ptr<RenderItem>> sourceRenderItems;
    for (const auto& entry : renderSources) {
        if (entry.second->isEnabled()) {
            sourceRenderItems.emplace_back(entry.second->createRenderItem());
        }
    }

    return std::make_unique<RenderTreeImpl>(
        std::move(renderTreeParameters),
        std::move(layerRenderItems),
        std::move(sourceRenderItems),
        *lineAtlas,
        *patternAtlas);
}

std::vector<Feature> RenderOrchestrator::queryRenderedFeatures(const ScreenLineString& geometry, const RenderedQueryOptions& options) const {
    std::unordered_map<std::string, const RenderLayer*> layers;
    if (options.layerIDs) {
        for (const auto& layerID : *options.layerIDs) {
            if (const RenderLayer* layer = getRenderLayer(layerID)) {
                layers.emplace(layer->getID(), layer);
            }
        }
    } else {
        for (const auto& entry : renderLayers) {
            layers.emplace(entry.second->getID(), entry.second.get());
        }
    }

    return queryRenderedFeatures(geometry, options, layers);
}
    
void RenderOrchestrator::queryRenderedSymbols(std::unordered_map<std::string, std::vector<Feature>>& resultsByLayer,
                                          const ScreenLineString& geometry,
                                          const std::unordered_map<std::string, const RenderLayer*>& layers,
                                          const RenderedQueryOptions& options) const {
    const auto hasCrossTileIndex = [] (const auto& pair) {
        return pair.second->baseImpl->getTypeInfo()->crossTileIndex == style::LayerTypeInfo::CrossTileIndex::Required;
    };

    std::unordered_map<std::string, const RenderLayer*> crossTileSymbolIndexLayers;
    std::copy_if(layers.begin(),
                 layers.end(),
                 std::inserter(crossTileSymbolIndexLayers, crossTileSymbolIndexLayers.begin()),
                 hasCrossTileIndex);

    if (crossTileSymbolIndexLayers.empty()) {
        return;
    }

    auto renderedSymbols = placement->getCollisionIndex().queryRenderedSymbols(geometry);
    std::vector<std::reference_wrapper<const RetainedQueryData>> bucketQueryData;
    for (auto entry : renderedSymbols) {
        bucketQueryData.emplace_back(placement->getQueryData(entry.first));
    }
    // Although symbol query is global, symbol results are only sortable within a bucket
    // For a predictable global sort renderItems, we sort the buckets based on their corresponding tile position
    std::sort(bucketQueryData.begin(), bucketQueryData.end(), [](const RetainedQueryData& a, const RetainedQueryData& b) {
        return
            std::tie(a.tileID.canonical.z, a.tileID.canonical.y, a.tileID.wrap, a.tileID.canonical.x) <
            std::tie(b.tileID.canonical.z, b.tileID.canonical.y, b.tileID.wrap, b.tileID.canonical.x);
    });
    
    for (auto wrappedQueryData : bucketQueryData) {
        auto& queryData = wrappedQueryData.get();
        auto bucketSymbols = queryData.featureIndex->lookupSymbolFeatures(renderedSymbols[queryData.bucketInstanceId],
                                                                          options,
                                                                          crossTileSymbolIndexLayers,
                                                                          queryData.tileID,
                                                                          queryData.featureSortOrder);
        
        for (auto layer : bucketSymbols) {
            auto& resultFeatures = resultsByLayer[layer.first];
            std::move(layer.second.begin(), layer.second.end(), std::inserter(resultFeatures, resultFeatures.end()));
        }
    }
}

std::vector<Feature> RenderOrchestrator::queryRenderedFeatures(const ScreenLineString& geometry, const RenderedQueryOptions& options, const std::unordered_map<std::string, const RenderLayer*>& layers) const {
    std::unordered_set<std::string> sourceIDs;
    std::unordered_map<std::string, const RenderLayer*> filteredLayers;
    for (const auto& pair : layers) {
        if (!pair.second->needsRendering() || !pair.second->supportsZoom(zoomHistory.lastZoom)) {
            continue;
        }
        filteredLayers.emplace(pair);
        sourceIDs.emplace(pair.second->baseImpl->source);
    }

    mat4 projMatrix;
    transformState.getProjMatrix(projMatrix);

    std::unordered_map<std::string, std::vector<Feature>> resultsByLayer;
    for (const auto& sourceID : sourceIDs) {
        if (RenderSource* renderSource = getRenderSource(sourceID)) {
            auto sourceResults = renderSource->queryRenderedFeatures(geometry, transformState, filteredLayers, options, projMatrix);
            std::move(sourceResults.begin(), sourceResults.end(), std::inserter(resultsByLayer, resultsByLayer.begin()));
        }
    }
    
    queryRenderedSymbols(resultsByLayer, geometry, filteredLayers, options);

    std::vector<Feature> result;

    if (resultsByLayer.empty()) {
        return result;
    }

    // Combine all results based on the style layer renderItems.
    for (const auto& pair : filteredLayers) {
        auto it = resultsByLayer.find(pair.second->baseImpl->id);
        if (it != resultsByLayer.end()) {
            std::move(it->second.begin(), it->second.end(), std::back_inserter(result));
        }
    }

    return result;
}

std::vector<Feature> RenderOrchestrator::queryShapeAnnotations(const ScreenLineString& geometry) const {
    assert(LayerManager::annotationsEnabled);
    std::unordered_map<std::string, const RenderLayer*> shapeAnnotationLayers;
    RenderedQueryOptions options;
    for (const auto& layerImpl : *layerImpls) {
        if (std::mismatch(layerImpl->id.begin(), layerImpl->id.end(),
                          AnnotationManager::ShapeLayerID.begin(), AnnotationManager::ShapeLayerID.end()).second == AnnotationManager::ShapeLayerID.end()) {
            if (const RenderLayer* layer = getRenderLayer(layerImpl->id)) {
                shapeAnnotationLayers.emplace(layer->getID(), layer);
            }
        }
    }

    return queryRenderedFeatures(geometry, options, shapeAnnotationLayers);
}

std::vector<Feature> RenderOrchestrator::querySourceFeatures(const std::string& sourceID, const SourceQueryOptions& options) const {
    const RenderSource* source = getRenderSource(sourceID);
    if (!source) return {};

    return source->querySourceFeatures(options);
}

FeatureExtensionValue RenderOrchestrator::queryFeatureExtensions(const std::string& sourceID,
                                                             const Feature& feature,
                                                             const std::string& extension,
                                                             const std::string& extensionField,
                                                             const optional<std::map<std::string, Value>>& args) const {
    if (RenderSource* renderSource = getRenderSource(sourceID)) {
        return renderSource->queryFeatureExtensions(feature, extension, extensionField, args);
    }
    return {};
}

void RenderOrchestrator::reduceMemoryUse() {
    filteredLayersForSource.shrink_to_fit();
    for (const auto& entry : renderSources) {
        entry.second->reduceMemoryUse();
    }
    imageManager->reduceMemoryUse();
    observer->onInvalidate();
}

void RenderOrchestrator::dumpDebugLogs() {
    for (const auto& entry : renderSources) {
        entry.second->dumpDebugLogs();
    }

    imageManager->dumpDebugLogs();
}

RenderLayer* RenderOrchestrator::getRenderLayer(const std::string& id) {
    auto it = renderLayers.find(id);
    return it != renderLayers.end() ? it->second.get() : nullptr;
}

const RenderLayer* RenderOrchestrator::getRenderLayer(const std::string& id) const {
    auto it = renderLayers.find(id);
    return it != renderLayers.end() ? it->second.get() : nullptr;
}

RenderSource* RenderOrchestrator::getRenderSource(const std::string& id) const {
    auto it = renderSources.find(id);
    return it != renderSources.end() ? it->second.get() : nullptr;
}

bool RenderOrchestrator::hasTransitions(TimePoint timePoint) const {
    if (renderLight.hasTransition()) {
        return true;
    }

    for (const auto& entry : renderLayers) {
        if (entry.second->hasTransition()) {
            return true;
        }
    }

    if (placement->hasTransitions(timePoint)) {
        return true;
    }

    for (const auto& entry : renderSources) {
        if (entry.second->hasFadingTiles()) {
            return true;
        }
    }

    return false;
}

bool RenderOrchestrator::isLoaded() const {
    for (const auto& entry: renderSources) {
        if (!entry.second->isLoaded()) {
            return false;
        }
    }

    if (!imageManager->isLoaded()) {
        return false;
    }

    return true;
}

void RenderOrchestrator::onGlyphsError(const FontStack& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) {
    Log::Error(Event::Style, "Failed to load glyph range %d-%d for font stack %s: %s",
               glyphRange.first, glyphRange.second, fontStackToString(fontStack).c_str(), util::toString(error).c_str());
    observer->onResourceError(error);
}

void RenderOrchestrator::onTileError(RenderSource& source, const OverscaledTileID& tileID, std::exception_ptr error) {
    Log::Error(Event::Style, "Failed to load tile %s for source %s: %s",
               util::toString(tileID).c_str(), source.baseImpl->id.c_str(), util::toString(error).c_str());
    observer->onResourceError(error);
}

void RenderOrchestrator::onTileChanged(RenderSource&, const OverscaledTileID&) {
    observer->onInvalidate();
}

void RenderOrchestrator::onStyleImageMissing(const std::string& id, std::function<void()> done) {
    observer->onStyleImageMissing(id, std::move(done));
}

void RenderOrchestrator::onRemoveUnusedStyleImages(const std::vector<std::string>& unusedImageIDs) {
    observer->onRemoveUnusedStyleImages(unusedImageIDs);
}

} // namespace mbgl
