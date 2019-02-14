#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/layermanager/layer_manager.hpp>
#include <mbgl/renderer/renderer_impl.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/transition_parameters.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/style_diff.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/gfx/render_pass.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/gfx/renderable.hpp>
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

Renderer::Impl::Impl(gfx::RendererBackend& backend_,
                     float pixelRatio_,
                     const optional<std::string> programCacheDir_,
                     const optional<std::string> localFontFamily_)
    : backend(backend_)
    , observer(&nullObserver())
    , pixelRatio(pixelRatio_)
    , programCacheDir(std::move(programCacheDir_))
    , localFontFamily(std::move(localFontFamily_))
    , imageManager(std::make_unique<ImageManager>())
    , lineAtlas(std::make_unique<LineAtlas>(Size{ 256, 512 }))
    , imageImpls(makeMutable<std::vector<Immutable<style::Image::Impl>>>())
    , sourceImpls(makeMutable<std::vector<Immutable<style::Source::Impl>>>())
    , layerImpls(makeMutable<std::vector<Immutable<style::Layer::Impl>>>())
    , renderLight(makeMutable<Light::Impl>())
    , placement(std::make_unique<Placement>(TransformState{}, MapMode::Static, TransitionOptions{}, true)) {
    imageManager->setObserver(this);
}

Renderer::Impl::~Impl() {
    assert(gfx::BackendScope::exists());

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

void Renderer::Impl::setObserver(RendererObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver();
}

void Renderer::Impl::render(const UpdateParameters& updateParameters) {
    if (!glyphManager) {
        glyphManager = std::make_unique<GlyphManager>(updateParameters.fileSource, std::make_unique<LocalGlyphRasterizer>(localFontFamily));
        glyphManager->setObserver(this);
    }
    const bool isMapModeContinuous = updateParameters.mode == MapMode::Continuous;

    if (!isMapModeContinuous) {
        // Reset zoom history state.
        zoomHistory.first = true;
    }

    assert(gfx::BackendScope::exists());
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
    }

    // Add added images to sprite atlas.
    for (const auto& entry : imageDiff.added) {
        imageManager->addImage(entry.second);
    }

    // Update changed images.
    for (const auto& entry : imageDiff.changed) {
        hasImageDiff = imageManager->updateImage(entry.second.after) || hasImageDiff;
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
    for (const auto& entry : renderLayers) {
        RenderLayer& layer = *entry.second;
        const bool layerAddedOrChanged = layerDiff.added.count(entry.first) || layerDiff.changed.count(entry.first);
        if (layerAddedOrChanged || zoomChanged || layer.hasTransition() || layer.hasCrossfade()) {
            layer.evaluate(evaluationParameters);
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

    if (!staticData) {
        staticData = std::make_unique<RenderStaticData>(backend.getContext(), pixelRatio, programCacheDir);
    }

    Color backgroundColor;

    struct RenderItem {
        RenderItem(RenderLayer& layer_, RenderSource* source_, uint32_t index_)
            : layer(layer_), source(source_), index(index_) {}
        std::reference_wrapper<RenderLayer> layer;
        RenderSource* source;
        uint32_t index;
        bool operator<(const RenderItem& other) const { return index < other.index; }
    };

    std::set<RenderItem> renderItems;
    std::vector<const RenderLayerSymbolInterface*> renderItemsWithSymbols;
    auto renderItemsEmplaceHint = renderItems.begin();

    // Update all sources and initialize renderItems.
    staticData->has3D = false;
    for (const auto& sourceImpl : *sourceImpls) {
        RenderSource* source = renderSources.at(sourceImpl->id).get();
        std::vector<Immutable<LayerProperties>> filteredLayersForSource;
        filteredLayersForSource.reserve(layerImpls->size());
        bool sourceNeedsRendering = false;
        bool sourceNeedsRelayout = false;       
        
        uint32_t index = 0u;
        const auto begin = layerImpls->begin();
        const auto end = layerImpls->end();
        for (auto it = begin; it != end; ++it, ++index) {
            const Immutable<Layer::Impl>& layerImpl = *it;
            RenderLayer* layer = getRenderLayer(layerImpl->id);
            const auto* layerInfo = layerImpl->getTypeInfo();
            const bool layerNeedsRendering = layer->needsRendering();
            const bool zoomFitsLayer = layer->supportsZoom(zoomHistory.lastZoom);
            staticData->has3D = (staticData->has3D || layerInfo->pass3d == LayerTypeInfo::Pass3D::Required);

            if (layerInfo->source != LayerTypeInfo::Source::NotRequired) {
                if (layerImpl->source == sourceImpl->id) {
                    sourceNeedsRelayout = (sourceNeedsRelayout || hasImageDiff || hasLayoutDifference(layerDiff, layerImpl->id));
                    if (layerNeedsRendering) {
                        filteredLayersForSource.push_back(layer->evaluatedProperties);
                        if (zoomFitsLayer) {
                            sourceNeedsRendering = true;
                            renderItemsEmplaceHint = renderItems.emplace_hint(renderItemsEmplaceHint, *layer, source, index);
                        }
                    }
                }
                continue;
            } 

            // Handle layers without source.
            if (layerNeedsRendering && zoomFitsLayer && sourceImpl.get() == sourceImpls->at(0).get()) {
                if (!backend.contextIsShared() && layerImpl.get() == layerImpls->at(0).get()) {
                    const auto& solidBackground = layer->getSolidBackground();
                    if (solidBackground) {
                        backgroundColor = *solidBackground;
                        continue; // This layer is shown with background color, and it shall not be added to render items. 
                    }
                }
                renderItemsEmplaceHint = renderItems.emplace_hint(renderItemsEmplaceHint, *layer, nullptr, index);
            }
        }
        source->update(sourceImpl,
                       filteredLayersForSource,
                       sourceNeedsRendering,
                       sourceNeedsRelayout,
                       tileParameters);
    }

    const bool loaded = updateParameters.styleLoaded && isLoaded();
    if (!isMapModeContinuous && !loaded) {
        return;
    }

    if (renderState == RenderState::Never) {
        observer->onWillStartRenderingMap();
    }

    observer->onWillStartRenderingFrame();

    // Set render tiles to the render items.
    for (auto& renderItem : renderItems) {
        if (!renderItem.source) {
            continue;
        }
        RenderLayer& renderLayer = renderItem.layer;
        renderLayer.setRenderTiles(renderItem.source->getRenderTiles(), updateParameters.transformState);
        if (const RenderLayerSymbolInterface* symbolLayer = renderLayer.getSymbolInterface()) {
            renderItemsWithSymbols.push_back(symbolLayer);
        }
    }

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

        for (auto it = renderItemsWithSymbols.rbegin(); it != renderItemsWithSymbols.rend(); ++it) {
            const RenderLayerSymbolInterface *symbolLayer = *it;
            if (crossTileSymbolIndex.addLayer(*symbolLayer, updateParameters.transformState.getLatLng().longitude())) symbolBucketsChanged = true;

            if (placementChanged) {
                usedSymbolLayers.insert(symbolLayer->layerID());
                mat4 projMatrix;
                updateParameters.transformState.getProjMatrix(projMatrix);
                placement->placeLayer(*symbolLayer, projMatrix, updateParameters.debugOptions & MapDebugOptions::Collision);
            }
        }

        if (placementChanged) {
            placement->commit(updateParameters.timePoint);
            crossTileSymbolIndex.pruneUnusedLayers(usedSymbolLayers);
            updateFadingTiles();
        } else {
            placement->setStale();
        }


        if (placementChanged || symbolBucketsChanged) {
            for (auto it = renderItemsWithSymbols.rbegin(); it != renderItemsWithSymbols.rend(); ++it) {
                const RenderLayerSymbolInterface *symbolLayer = *it;
                placement->updateLayerOpacities(*symbolLayer);
            }
        }
    }

    PaintParameters parameters {
        backend.getContext(),
        pixelRatio,
        backend,
        updateParameters,
        renderLight.getEvaluated(),
        *staticData,
        *imageManager,
        *lineAtlas,
        placement->getVariableOffsets()
    };

    parameters.symbolFadeChange = placement->symbolFadeChange(updateParameters.timePoint);

    // - UPLOAD PASS -------------------------------------------------------------------------------
    // Uploads all required buffers and images before we do any actual rendering.
    {
        const auto debugGroup(parameters.encoder->createDebugGroup("upload"));

        parameters.imageManager.upload(parameters.context);
        parameters.lineAtlas.upload(parameters.context);
        
        // Update all clipping IDs + upload buckets.
        for (const auto& entry : renderSources) {
            if (entry.second->isEnabled()) {
                entry.second->startRender(parameters);
            }
        }
    }

    // - 3D PASS -------------------------------------------------------------------------------------
    // Renders any 3D layers bottom-to-top to unique FBOs with texture attachments, but share the same
    // depth rbo between them.
    if (parameters.staticData.has3D) {
        parameters.staticData.backendSize = parameters.backend.getDefaultRenderable().getSize();

        const auto debugGroup(parameters.encoder->createDebugGroup("3d"));
        parameters.pass = RenderPass::Pass3D;

        if (!parameters.staticData.depthRenderbuffer ||
            parameters.staticData.depthRenderbuffer->getSize() != parameters.staticData.backendSize) {
            parameters.staticData.depthRenderbuffer =
                parameters.context.createRenderbuffer<gfx::RenderbufferPixelType::Depth>(parameters.staticData.backendSize);
        }
        parameters.staticData.depthRenderbuffer->setShouldClear(true);

        uint32_t i = static_cast<uint32_t>(renderItems.size()) - 1;
        for (auto it = renderItems.begin(); it != renderItems.end(); ++it, --i) {
            parameters.currentLayer = i;
            RenderLayer& renderLayer = it->layer;
            if (renderLayer.hasRenderPass(parameters.pass)) {
                const auto layerDebugGroup(parameters.encoder->createDebugGroup(renderLayer.getID().c_str()));
                renderLayer.render(parameters, it->source);
            }
        }
    }

    // - CLEAR -------------------------------------------------------------------------------------
    // Renders the backdrop of the OpenGL view. This also paints in areas where we don't have any
    // tiles whatsoever.
    {
        optional<Color> color;
        if (parameters.debugOptions & MapDebugOptions::Overdraw) {
            color = Color::black();
        } else if (!backend.contextIsShared()) {
            color = backgroundColor;
        }
        parameters.renderPass = parameters.encoder->createRenderPass("main buffer", { parameters.backend.getDefaultRenderable(), color, 1, 0 });
    }

    // Actually render the layers

    parameters.depthRangeSize = 1 - (renderItems.size() + 2) * parameters.numSublayers * parameters.depthEpsilon;

    // - OPAQUE PASS -------------------------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque objects first.
    {
        parameters.pass = RenderPass::Opaque;
        const auto debugGroup(parameters.renderPass->createDebugGroup("opaque"));

        uint32_t i = 0;
        for (auto it = renderItems.rbegin(); it != renderItems.rend(); ++it, ++i) {
            parameters.currentLayer = i;
            RenderLayer& renderLayer = it->layer;
            if (renderLayer.hasRenderPass(parameters.pass)) {
                const auto layerDebugGroup(parameters.renderPass->createDebugGroup(renderLayer.getID().c_str()));
                renderLayer.render(parameters, it->source);
            }
        }
    }

    // - TRANSLUCENT PASS --------------------------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render bottom-to-top.
    {
        parameters.pass = RenderPass::Translucent;
        const auto debugGroup(parameters.renderPass->createDebugGroup("translucent"));

        uint32_t i = static_cast<uint32_t>(renderItems.size()) - 1;
        for (auto it = renderItems.begin(); it != renderItems.end(); ++it, --i) {
            parameters.currentLayer = i;
            RenderLayer& renderLayer = it->layer;
            if (renderLayer.hasRenderPass(parameters.pass)) {
                const auto layerDebugGroup(parameters.renderPass->createDebugGroup(renderLayer.getID().c_str()));
                renderLayer.render(parameters, it->source);
            }
        }
    }

    // - DEBUG PASS --------------------------------------------------------------------------------
    // Renders debug overlays.
    {
        const auto debugGroup(parameters.renderPass->createDebugGroup("debug"));

        // Finalize the rendering, e.g. by calling debug render calls per tile.
        // This guarantees that we have at least one function per tile called.
        // When only rendering layers via the stylesheet, it's possible that we don't
        // ever visit a tile during rendering.
        for (const auto& entry : renderSources) {
            if (entry.second->isEnabled()) {
                entry.second->finishRender(parameters);
            }
        }
    }

#if not defined(NDEBUG)
    if (parameters.debugOptions & MapDebugOptions::StencilClip) {
        // Render tile clip boundaries, using stencil buffer to calculate fill color.
        parameters.context.visualizeStencilBuffer();
    } else if (parameters.debugOptions & MapDebugOptions::DepthBuffer) {
        // Render the depth buffer.
        parameters.context.visualizeDepthBuffer(parameters.depthRangeSize);
    }
#endif

    observer->onDidFinishRenderingFrame(
        loaded ? RendererObserver::RenderMode::Full : RendererObserver::RenderMode::Partial,
        isMapModeContinuous && hasTransitions(parameters.timePoint)
    );

    if (!loaded) {
        renderState = RenderState::Partial;
    } else if (renderState != RenderState::Fully) {
        renderState = RenderState::Fully;
        observer->onDidFinishRenderingMap();
    }

    // CommandEncoder destructor submits render commands.
}

std::vector<Feature> Renderer::Impl::queryRenderedFeatures(const ScreenLineString& geometry, const RenderedQueryOptions& options) const {
    std::vector<const RenderLayer*> layers;
    if (options.layerIDs) {
        for (const auto& layerID : *options.layerIDs) {
            if (const RenderLayer* layer = getRenderLayer(layerID)) {
                layers.emplace_back(layer);
            }
        }
    } else {
        for (const auto& entry : renderLayers) {
            layers.emplace_back(entry.second.get());
        }
    }

    return queryRenderedFeatures(geometry, options, layers);
}
    
void Renderer::Impl::queryRenderedSymbols(std::unordered_map<std::string, std::vector<Feature>>& resultsByLayer,
                                          const ScreenLineString& geometry,
                                          const std::vector<const RenderLayer*>& layers,
                                          const RenderedQueryOptions& options) const {
    
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
                                                                          layers,
                                                                          queryData.tileID,
                                                                          queryData.featureSortOrder);
        
        for (auto layer : bucketSymbols) {
            auto& resultFeatures = resultsByLayer[layer.first];
            std::move(layer.second.begin(), layer.second.end(), std::inserter(resultFeatures, resultFeatures.end()));
        }
    }
}

std::vector<Feature> Renderer::Impl::queryRenderedFeatures(const ScreenLineString& geometry, const RenderedQueryOptions& options, const std::vector<const RenderLayer*>& layers) const {
    std::unordered_set<std::string> sourceIDs;
    for (const RenderLayer* layer : layers) {
        sourceIDs.emplace(layer->baseImpl->source);
    }

    mat4 projMatrix;
    transformState.getProjMatrix(projMatrix);

    std::unordered_map<std::string, std::vector<Feature>> resultsByLayer;
    for (const auto& sourceID : sourceIDs) {
        if (RenderSource* renderSource = getRenderSource(sourceID)) {
            auto sourceResults = renderSource->queryRenderedFeatures(geometry, transformState, layers, options, projMatrix);
            std::move(sourceResults.begin(), sourceResults.end(), std::inserter(resultsByLayer, resultsByLayer.begin()));
        }
    }
    
    queryRenderedSymbols(resultsByLayer, geometry, layers, options);

    std::vector<Feature> result;

    if (resultsByLayer.empty()) {
        return result;
    }

    // Combine all results based on the style layer renderItems.
    for (const auto& layerImpl : *layerImpls) {
        const RenderLayer* layer = getRenderLayer(layerImpl->id);
        if (!layer->needsRendering() || !layer->supportsZoom(zoomHistory.lastZoom)) {
            continue;
        }

        auto it = resultsByLayer.find(layer->baseImpl->id);
        if (it != resultsByLayer.end()) {
            std::move(it->second.begin(), it->second.end(), std::back_inserter(result));
        }
    }

    return result;
}

std::vector<Feature> Renderer::Impl::queryShapeAnnotations(const ScreenLineString& geometry) const {
    assert(LayerManager::annotationsEnabled);
    std::vector<const RenderLayer*> shapeAnnotationLayers;
    RenderedQueryOptions options;
    for (const auto& layerImpl : *layerImpls) {
        if (std::mismatch(layerImpl->id.begin(), layerImpl->id.end(),
                          AnnotationManager::ShapeLayerID.begin(), AnnotationManager::ShapeLayerID.end()).second == AnnotationManager::ShapeLayerID.end()) {
            if (const RenderLayer* layer = getRenderLayer(layerImpl->id)) {
                shapeAnnotationLayers.emplace_back(layer);
            }
        }
    }

    return queryRenderedFeatures(geometry, options, shapeAnnotationLayers);
}

std::vector<Feature> Renderer::Impl::querySourceFeatures(const std::string& sourceID, const SourceQueryOptions& options) const {
    const RenderSource* source = getRenderSource(sourceID);
    if (!source) return {};

    return source->querySourceFeatures(options);
}

FeatureExtensionValue Renderer::Impl::queryFeatureExtensions(const std::string& sourceID,
                                                             const Feature& feature,
                                                             const std::string& extension,
                                                             const std::string& extensionField,
                                                             const optional<std::map<std::string, Value>>& args) const {
    if (RenderSource* renderSource = getRenderSource(sourceID)) {
        return renderSource->queryFeatureExtensions(feature, extension, extensionField, args);
    }
    return {};
}

void Renderer::Impl::reduceMemoryUse() {
    assert(gfx::BackendScope::exists());
    for (const auto& entry : renderSources) {
        entry.second->reduceMemoryUse();
    }
    backend.getContext().performCleanup();
    observer->onInvalidate();
    if (imageManager) {
        imageManager->reduceMemoryUse();
    }
}

void Renderer::Impl::dumpDebugLogs() {
    for (const auto& entry : renderSources) {
        entry.second->dumpDebugLogs();
    }

    imageManager->dumpDebugLogs();
}

RenderLayer* Renderer::Impl::getRenderLayer(const std::string& id) {
    auto it = renderLayers.find(id);
    return it != renderLayers.end() ? it->second.get() : nullptr;
}

const RenderLayer* Renderer::Impl::getRenderLayer(const std::string& id) const {
    auto it = renderLayers.find(id);
    return it != renderLayers.end() ? it->second.get() : nullptr;
}

RenderSource* Renderer::Impl::getRenderSource(const std::string& id) const {
    auto it = renderSources.find(id);
    return it != renderSources.end() ? it->second.get() : nullptr;
}

bool Renderer::Impl::hasTransitions(TimePoint timePoint) const {
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
    
    if (fadingTiles) {
        return true;
    }

    return false;
}

void Renderer::Impl::updateFadingTiles() {
    fadingTiles = false;
    for (auto& source : renderSources) {
        for (auto& renderTile : source.second->getRenderTiles()) {
            Tile& tile = renderTile.get().tile;
            if (tile.holdForFade()) {
                fadingTiles = true;
                tile.performedFadePlacement();
            }
        }
    }
}

bool Renderer::Impl::isLoaded() const {
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

void Renderer::Impl::onGlyphsError(const FontStack& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) {
    Log::Error(Event::Style, "Failed to load glyph range %d-%d for font stack %s: %s",
               glyphRange.first, glyphRange.second, fontStackToString(fontStack).c_str(), util::toString(error).c_str());
    observer->onResourceError(error);
}

void Renderer::Impl::onTileError(RenderSource& source, const OverscaledTileID& tileID, std::exception_ptr error) {
    Log::Error(Event::Style, "Failed to load tile %s for source %s: %s",
               util::toString(tileID).c_str(), source.baseImpl->id.c_str(), util::toString(error).c_str());
    observer->onResourceError(error);
}

void Renderer::Impl::onTileChanged(RenderSource&, const OverscaledTileID&) {
    observer->onInvalidate();
}

void Renderer::Impl::onStyleImageMissing(const std::string& id, std::function<void()> done) {
    observer->onStyleImageMissing(id, std::move(done));
}

} // namespace mbgl
