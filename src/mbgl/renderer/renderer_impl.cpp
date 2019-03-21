#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/layermanager/layer_manager.hpp>
#include <mbgl/renderer/renderer_impl.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
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
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/debugging.hpp>
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

Renderer::Impl::Impl(RendererBackend& backend_,
                     float pixelRatio_,
                     Scheduler& scheduler_,
                     GLContextMode contextMode_,
                     const optional<std::string> programCacheDir_,
                     const optional<std::string> localFontFamily_)
    : backend(backend_)
    , scheduler(scheduler_)
    , observer(&nullObserver())
    , contextMode(contextMode_)
    , pixelRatio(pixelRatio_)
    , programCacheDir(std::move(programCacheDir_))
    , localFontFamily(std::move(localFontFamily_))
    , lineAtlas(std::make_unique<LineAtlas>(Size{ 256, 512 }))
    , imageImpls(makeMutable<std::vector<Immutable<style::Image::Impl>>>())
    , sourceImpls(makeMutable<std::vector<Immutable<style::Source::Impl>>>())
    , layerImpls(makeMutable<std::vector<Immutable<style::Layer::Impl>>>())
    , renderLight(makeMutable<Light::Impl>())
    , placement(std::make_unique<Placement>(TransformState{}, MapMode::Static, TransitionOptions{}, true)) {}

Renderer::Impl::~Impl() {
    assert(BackendScope::exists());

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

    if (!imageManager) {
        imageManager = std::make_unique<ImageManager>();
    }

    if (updateParameters.mode != MapMode::Continuous) {
        // Reset zoom history state.
        zoomHistory.first = true;
    }
    
    assert(BackendScope::exists());
    if (LayerManager::annotationsEnabled) {
        updateParameters.annotationManager.updateData();
    }

    const bool zoomChanged = zoomHistory.update(updateParameters.transformState.getZoom(), updateParameters.timePoint);

    const bool isMapModeContinuous = updateParameters.mode == MapMode::Continuous;

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
        scheduler,
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
        imageManager->updateImage(entry.second.after);
    }

    imageManager->setLoaded(updateParameters.spriteLoaded);


    const LayerDifference layerDiff = diffLayers(layerImpls, updateParameters.layers);
    layerImpls = updateParameters.layers;

    // Remove render layers for removed layers.
    for (const auto& entry : layerDiff.removed) {
        renderLayers.erase(entry.first);
    }

    // Create render layers for newly added layers.
    for (const auto& entry : layerDiff.added) {
        renderLayers.emplace(entry.first, LayerManager::get()->createRenderLayer(entry.second));
    }

    // Update render layers for changed layers.
    for (const auto& entry : layerDiff.changed) {
        renderLayers.at(entry.first)->setImpl(entry.second.after);
    }

    if (!layerDiff.removed.empty() || !layerDiff.added.empty() || !layerDiff.changed.empty()) {
        glyphManager->evict(fontStacks(*updateParameters.layers));
    }

    // Update layers for class and zoom changes.
    for (const auto& entry : renderLayers) {
        RenderLayer& layer = *entry.second;
        const bool layerAdded = layerDiff.added.count(entry.first);
        const bool layerChanged = layerDiff.changed.count(entry.first);

        if (layerAdded || layerChanged) {
            layer.transition(transitionParameters);
            layer.update();
        }

        if (layerAdded || layerChanged || zoomChanged || layer.hasTransition() || layer.hasCrossfade()) {
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

    const bool hasImageDiff = !(imageDiff.added.empty() && imageDiff.removed.empty() && imageDiff.changed.empty());

    // Update all sources.
    for (const auto& source : *sourceImpls) {
        std::vector<Immutable<Layer::Impl>> filteredLayers;
        bool needsRendering = false;
        bool needsRelayout = false;

        for (const auto& layer : *layerImpls) {

            if (layer->getTypeInfo()->source == LayerTypeInfo::Source::NotRequired
                    || layer->source != source->id) {
                continue;
            }

            if (!needsRendering && getRenderLayer(layer->id)->needsRendering(zoomHistory.lastZoom)) {
                needsRendering = true;
            }

            if (!needsRelayout && (hasImageDiff || hasLayoutDifference(layerDiff, layer->id))) {
                needsRelayout = true;
            }

            filteredLayers.push_back(layer);
        }

        renderSources.at(source->id)->update(source,
                                             filteredLayers,
                                             needsRendering,
                                             needsRelayout,
                                             tileParameters);
    }

    transformState = updateParameters.transformState;

    if (!staticData) {
        staticData = std::make_unique<RenderStaticData>(backend.getContext(), pixelRatio, programCacheDir);
    }

    PaintParameters parameters {
        backend.getContext(),
        pixelRatio,
        contextMode,
        backend,
        updateParameters,
        renderLight.getEvaluated(),
        *staticData,
        *imageManager,
        *lineAtlas
    };

    bool loaded = updateParameters.styleLoaded && isLoaded();
    if (updateParameters.mode != MapMode::Continuous && !loaded) {
        return;
    }

    if (renderState == RenderState::Never) {
        observer->onWillStartRenderingMap();
    }

    observer->onWillStartRenderingFrame();

    backend.updateAssumedState();

    // TODO: remove cast
    gl::Context& glContext = reinterpret_cast<gl::Context&>(parameters.context);

    if (parameters.contextMode == GLContextMode::Shared) {
        glContext.setDirtyState();
    }

    Color backgroundColor;

    struct RenderItem {
        RenderItem(RenderLayer& layer_, RenderSource* source_)
            : layer(layer_), source(source_) {}
        RenderLayer& layer;
        RenderSource* source;
    };

    std::vector<RenderItem> order;

    for (auto& layerImpl : *layerImpls) {
        RenderLayer* layer = getRenderLayer(layerImpl->id);
        assert(layer);

        parameters.staticData.has3D |=
                (layerImpl->getTypeInfo()->pass3d == LayerTypeInfo::Pass3D::Required);

        if (!layer->needsRendering(zoomHistory.lastZoom)) {
            continue;
        }

        if (parameters.contextMode == GLContextMode::Unique
            && layerImpl.get() == layerImpls->at(0).get()) {
            const auto& solidBackground = layer->getSolidBackground();
            if (solidBackground) {
                backgroundColor = *solidBackground;
                continue;
            }
        }

        if (layerImpl->getTypeInfo()->source == LayerTypeInfo::Source::NotRequired) {
            order.emplace_back(*layer, nullptr);
            continue;
        }

        RenderSource* source = getRenderSource(layer->baseImpl->source);
        if (!source) {
            Log::Warning(Event::Render, "can't find source for layer '%s'", layer->getID().c_str());
            continue;
        }

        layer->setRenderTiles(source->getRenderTiles(), parameters.state);
        order.emplace_back(*layer, source);
    }

    {
        if (parameters.mapMode != MapMode::Continuous) {
            // TODO: Think about right way for symbol index to handle still rendering
            crossTileSymbolIndex.reset();
        }

        std::vector<RenderItem> renderItemsWithSymbols;
        std::copy_if(order.rbegin(), order.rend(), std::back_inserter(renderItemsWithSymbols),
                [](const auto& item) { return item.layer.getSymbolInterface() != nullptr; });

        bool symbolBucketsChanged = false;
        const bool placementChanged = !placement->stillRecent(parameters.timePoint);
        std::unique_ptr<Placement> newPlacement;
        std::set<std::string> usedSymbolLayers;

        if (placementChanged) {
            newPlacement = std::make_unique<Placement>(parameters.state, parameters.mapMode, updateParameters.transitionOptions, updateParameters.crossSourceCollisions);
        }

        for (const auto& item : renderItemsWithSymbols) {
            if (crossTileSymbolIndex.addLayer(*item.layer.getSymbolInterface(), parameters.state.getLatLng().longitude())) symbolBucketsChanged = true;

            if (newPlacement) {
                usedSymbolLayers.insert(item.layer.getID());
                newPlacement->placeLayer(*item.layer.getSymbolInterface(), parameters.projMatrix, parameters.debugOptions & MapDebugOptions::Collision);
            }
        }

        if (newPlacement) {
            newPlacement->commit(*placement, parameters.timePoint);
            crossTileSymbolIndex.pruneUnusedLayers(usedSymbolLayers);
            placement = std::move(newPlacement);
            updateFadingTiles();
        } else {
            placement->setStale();
        }

        parameters.symbolFadeChange = placement->symbolFadeChange(parameters.timePoint);

        if (placementChanged || symbolBucketsChanged) {
            for (const auto& item : renderItemsWithSymbols) {
                placement->updateLayerOpacities(*item.layer.getSymbolInterface());
            }
        }
    }

    // - UPLOAD PASS -------------------------------------------------------------------------------
    // Uploads all required buffers and images before we do any actual rendering.
    {
        MBGL_DEBUG_GROUP(parameters.context, "upload");

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
        parameters.staticData.backendSize = parameters.backend.getFramebufferSize();

        MBGL_DEBUG_GROUP(parameters.context, "3d");
        parameters.pass = RenderPass::Pass3D;

        if (!parameters.staticData.depthRenderbuffer ||
            parameters.staticData.depthRenderbuffer->size != parameters.staticData.backendSize) {
            parameters.staticData.depthRenderbuffer =
                glContext.createRenderbuffer<gl::RenderbufferType::DepthComponent>(parameters.staticData.backendSize);
        }
        parameters.staticData.depthRenderbuffer->shouldClear(true);

        uint32_t i = static_cast<uint32_t>(order.size()) - 1;
        for (auto it = order.begin(); it != order.end(); ++it, --i) {
            parameters.currentLayer = i;
            if (it->layer.hasRenderPass(parameters.pass)) {
                MBGL_DEBUG_GROUP(parameters.context, it->layer.getID());
                it->layer.render(parameters, it->source);
            }
        }
    }

    // - CLEAR -------------------------------------------------------------------------------------
    // Renders the backdrop of the OpenGL view. This also paints in areas where we don't have any
    // tiles whatsoever.
    {
        using namespace gl::value;

        MBGL_DEBUG_GROUP(parameters.context, "clear");
        parameters.backend.bind();
        if (parameters.debugOptions & MapDebugOptions::Overdraw) {
            glContext.clear(Color::black(), ClearDepth::Default, ClearStencil::Default);
        } else if (parameters.contextMode == GLContextMode::Shared) {
            glContext.clear({}, ClearDepth::Default, ClearStencil::Default);
        } else {
            glContext.clear(backgroundColor, ClearDepth::Default, ClearStencil::Default);
        }
    }

    // - CLIPPING MASKS ----------------------------------------------------------------------------
    // Draws the clipping masks to the stencil buffer.
    {
        MBGL_DEBUG_GROUP(parameters.context, "clipping masks");

        static const Properties<>::PossiblyEvaluated properties {};
        static const ClippingMaskProgram::Binders paintAttributeData(properties, 0);

        for (const auto& clipID : parameters.clipIDGenerator.getClipIDs()) {
            auto& program = parameters.staticData.programs.clippingMask;

            program.draw(
                parameters.context,
                gfx::Triangles(),
                gfx::DepthMode::disabled(),
                gfx::StencilMode {
                    gfx::StencilMode::Always(),
                    static_cast<int32_t>(clipID.second.reference.to_ulong()),
                    0b11111111,
                    gfx::StencilOpType::Keep,
                    gfx::StencilOpType::Keep,
                    gfx::StencilOpType::Replace
                },
                gfx::ColorMode::disabled(),
                gfx::CullFaceMode::disabled(),
                parameters.staticData.quadTriangleIndexBuffer,
                parameters.staticData.tileTriangleSegments,
                program.computeAllUniformValues(
                    ClippingMaskProgram::LayoutUniformValues {
                        uniforms::matrix::Value( parameters.matrixForTile(clipID.first) ),
                    },
                    paintAttributeData,
                    properties,
                    parameters.state.getZoom()
                ),
                program.computeAllAttributeBindings(
                    parameters.staticData.tileVertexBuffer,
                    paintAttributeData,
                    properties
                ),
                ClippingMaskProgram::TextureBindings{},
                "clipping"
            );
        }
    }

#if not MBGL_USE_GLES2 and not defined(NDEBUG)
    // Render tile clip boundaries, using stencil buffer to calculate fill color.
    if (parameters.debugOptions & MapDebugOptions::StencilClip) {
        glContext.setStencilMode(gfx::StencilMode::disabled());
        glContext.setDepthMode(gfx::DepthMode::disabled());
        glContext.setColorMode(gfx::ColorMode::unblended());
        glContext.program = 0;

        // Reset the value in case someone else changed it, or it's dirty.
        glContext.pixelTransferStencil = gl::value::PixelTransferStencil::Default;

        // Read the stencil buffer
        const auto viewport = glContext.viewport.getCurrentValue();
        auto image = glContext.readFramebuffer<AlphaImage, gfx::TexturePixelType::Stencil>(viewport.size, false);

        // Scale the Stencil buffer to cover the entire color space.
        auto it = image.data.get();
        auto end = it + viewport.size.width * viewport.size.height;
        const auto factor = 255.0f / *std::max_element(it, end);
        for (; it != end; ++it) {
            *it *= factor;
        }

        glContext.pixelZoom = { 1, 1 };
        glContext.rasterPos = { -1, -1, 0, 1 };
        glContext.drawPixels(image);

        return;
    }
#endif

    // Actually render the layers

    parameters.depthRangeSize = 1 - (order.size() + 2) * parameters.numSublayers * parameters.depthEpsilon;

    // - OPAQUE PASS -------------------------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque objects first.
    {
        parameters.pass = RenderPass::Opaque;
        MBGL_DEBUG_GROUP(parameters.context, "opaque");

        uint32_t i = 0;
        for (auto it = order.rbegin(); it != order.rend(); ++it, ++i) {
            parameters.currentLayer = i;
            if (it->layer.hasRenderPass(parameters.pass)) {
                MBGL_DEBUG_GROUP(parameters.context, it->layer.getID());
                it->layer.render(parameters, it->source);
            }
        }
    }

    // - TRANSLUCENT PASS --------------------------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render bottom-to-top.
    {
        parameters.pass = RenderPass::Translucent;
        MBGL_DEBUG_GROUP(parameters.context, "translucent");

        uint32_t i = static_cast<uint32_t>(order.size()) - 1;
        for (auto it = order.begin(); it != order.end(); ++it, --i) {
            parameters.currentLayer = i;
            if (it->layer.hasRenderPass(parameters.pass)) {
                MBGL_DEBUG_GROUP(parameters.context, it->layer.getID());
                it->layer.render(parameters, it->source);
            }
        }
    }

    // - DEBUG PASS --------------------------------------------------------------------------------
    // Renders debug overlays.
    {
        MBGL_DEBUG_GROUP(parameters.context, "debug");

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

#if not MBGL_USE_GLES2 and not defined(NDEBUG)
    // Render the depth buffer.
    if (parameters.debugOptions & MapDebugOptions::DepthBuffer) {
        glContext.setStencilMode(gfx::StencilMode::disabled());
        glContext.setDepthMode(gfx::DepthMode::disabled());
        glContext.setColorMode(gfx::ColorMode::unblended());
        glContext.program = 0;

        // Scales the values in the depth buffer so that they cover the entire grayscale range. This
        // makes it easier to spot tiny differences.
        const float base = 1.0f / (1.0f - parameters.depthRangeSize);
        glContext.pixelTransferDepth = { base, 1.0f - base };

        // Read the stencil buffer
        auto viewport = glContext.viewport.getCurrentValue();
        auto image = glContext.readFramebuffer<AlphaImage, gfx::TexturePixelType::Depth>(viewport.size, false);

        glContext.pixelZoom = { 1, 1 };
        glContext.rasterPos = { -1, -1, 0, 1 };
        glContext.drawPixels(image);
    }
#endif

    // TODO: Find a better way to unbind VAOs after we're done with them without introducing
    // unnecessary bind(0)/bind(N) sequences.
    {
        MBGL_DEBUG_GROUP(parameters.context, "cleanup");

        glContext.activeTextureUnit = 1;
        glContext.texture[1] = 0;
        glContext.activeTextureUnit = 0;
        glContext.texture[0] = 0;

        glContext.bindVertexArray = 0;
    }

    observer->onDidFinishRenderingFrame(
        loaded ? RendererObserver::RenderMode::Full : RendererObserver::RenderMode::Partial,
        updateParameters.mode == MapMode::Continuous && hasTransitions(parameters.timePoint)
    );

    if (!loaded) {
        renderState = RenderState::Partial;
    } else if (renderState != RenderState::Fully) {
        renderState = RenderState::Fully;
        observer->onDidFinishRenderingMap();
    }

    // Cleanup only after signaling completion
    glContext.performCleanup();
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
        bucketQueryData.push_back(placement->getQueryData(entry.first));
    }
    // Although symbol query is global, symbol results are only sortable within a bucket
    // For a predictable global sort order, we sort the buckets based on their corresponding tile position
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

    // Combine all results based on the style layer order.
    for (const auto& layerImpl : *layerImpls) {
        const RenderLayer* layer = getRenderLayer(layerImpl->id);
        if (!layer->needsRendering(zoomHistory.lastZoom)) {
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
    assert(BackendScope::exists());
    for (const auto& entry : renderSources) {
        entry.second->reduceMemoryUse();
    }
    backend.getContext().performCleanup();
    observer->onInvalidate();
}

void Renderer::Impl::dumDebugLogs() {
    for (const auto& entry : renderSources) {
        entry.second->dumpDebugLogs();
    }

    if (imageManager) {
        imageManager->dumpDebugLogs();
    }
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

    if (!imageManager || !imageManager->isLoaded()) {
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

} // namespace mbgl
