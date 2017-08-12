#include <mbgl/renderer/renderer_impl.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/transition_parameters.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/renderer/layers/render_circle_layer.hpp>
#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/style_diff.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/gl/debugging.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/sprite/sprite_loader.hpp>
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
                     FileSource& fileSource_,
                     Scheduler& scheduler_,
                     GLContextMode contextMode_,
                     const optional<std::string> programCacheDir_)
    : backend(backend_)
    , scheduler(scheduler_)
    , fileSource(fileSource_)
    , observer(&nullObserver())
    , contextMode(contextMode_)
    , pixelRatio(pixelRatio_)
    , programCacheDir(programCacheDir_)
    , glyphManager(std::make_unique<GlyphManager>(fileSource))
    , imageManager(std::make_unique<ImageManager>())
    , lineAtlas(std::make_unique<LineAtlas>(Size{ 256, 512 }))
    , imageImpls(makeMutable<std::vector<Immutable<style::Image::Impl>>>())
    , sourceImpls(makeMutable<std::vector<Immutable<style::Source::Impl>>>())
    , layerImpls(makeMutable<std::vector<Immutable<style::Layer::Impl>>>())
    , renderLight(makeMutable<Light::Impl>()) {
    glyphManager->setObserver(this);
}

Renderer::Impl::~Impl() {
    assert(BackendScope::exists());
};

void Renderer::Impl::setObserver(RendererObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver();
}

void Renderer::Impl::render(const UpdateParameters& updateParameters) {
    // Don't load/render anyting in still mode until explicitly requested.
    if (updateParameters.mode == MapMode::Still && !updateParameters.stillImageRequest) return;
    
    assert(BackendScope::exists());

    const bool zoomChanged = zoomHistory.update(updateParameters.transformState.getZoom(), updateParameters.timePoint);

    const TransitionParameters transitionParameters {
        updateParameters.timePoint,
        updateParameters.mode == MapMode::Continuous ? updateParameters.transitionOptions : TransitionOptions()
    };

    const PropertyEvaluationParameters evaluationParameters {
        zoomHistory,
        updateParameters.timePoint,
        updateParameters.mode == MapMode::Continuous ? util::DEFAULT_TRANSITION_DURATION : Duration::zero()
    };

    const TileParameters tileParameters {
        updateParameters.pixelRatio,
        updateParameters.debugOptions,
        updateParameters.transformState,
        scheduler,
        fileSource,
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
        renderLayers.emplace(entry.first, RenderLayer::create(entry.second));
    }

    // Update render layers for changed layers.
    for (const auto& entry : layerDiff.changed) {
        renderLayers.at(entry.first)->setImpl(entry.second.after);
    }

    // Update layers for class and zoom changes.
    for (const auto& entry : renderLayers) {
        RenderLayer& layer = *entry.second;
        const bool layerAdded = layerDiff.added.count(entry.first);
        const bool layerChanged = layerDiff.changed.count(entry.first);

        if (layerAdded || layerChanged) {
            layer.transition(transitionParameters);
        }

        if (layerAdded || layerChanged || zoomChanged || layer.hasTransition()) {
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

    // Update all sources.
    for (const auto& source : *sourceImpls) {
        std::vector<Immutable<Layer::Impl>> filteredLayers;
        bool needsRendering = false;
        bool needsRelayout = false;

        for (const auto& layer : *layerImpls) {
            if (layer->type == LayerType::Background ||
                layer->type == LayerType::Custom ||
                layer->source != source->id) {
                continue;
            }

            if (!needsRendering && getRenderLayer(layer->id)->needsRendering(zoomHistory.lastZoom)) {
                needsRendering = true;
            }

            if (!needsRelayout && (
                hasLayoutDifference(layerDiff, layer->id) ||
                !imageDiff.added.empty() ||
                !imageDiff.removed.empty() ||
                !imageDiff.changed.empty())) {
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
        frameHistory,
        *imageManager,
        *lineAtlas
    };

    bool loaded = updateParameters.styleLoaded && isLoaded();
    if (updateParameters.mode == MapMode::Still && !loaded) {
        return;
    }

    if (renderState == RenderState::Never) {
        observer->onWillStartRenderingMap();
    }

    observer->onWillStartRenderingFrame();

    backend.updateAssumedState();

    if (parameters.contextMode == GLContextMode::Shared) {
        parameters.context.setDirtyState();
    }

    std::unordered_set<RenderSource*> sources;
    for (const auto& entry : renderSources) {
        if (entry.second->isEnabled()) {
            sources.insert(entry.second.get());
        }
    }

    Color backgroundColor;

    class RenderItem {
    public:
        RenderLayer& layer;
        RenderSource* source;
    };

    std::vector<RenderItem> order;

    for (auto& layerImpl : *layerImpls) {
        RenderLayer* layer = getRenderLayer(layerImpl->id);
        assert(layer);

        if (!layer->needsRendering(zoomHistory.lastZoom)) {
            continue;
        }

        if (const RenderBackgroundLayer* background = layer->as<RenderBackgroundLayer>()) {
            if (parameters.debugOptions & MapDebugOptions::Overdraw) {
                // We want to skip glClear optimization in overdraw mode.
                order.emplace_back(RenderItem { *layer, nullptr });
                continue;
            }
            const BackgroundPaintProperties::PossiblyEvaluated& paint = background->evaluated;
            if (layerImpl.get() == layerImpls->at(0).get() && paint.get<BackgroundPattern>().from.empty()) {
                // This is a solid background. We can use glClear().
                backgroundColor = paint.get<BackgroundColor>() * paint.get<BackgroundOpacity>();
            } else {
                // This is a textured background, or not the bottommost layer. We need to render it with a quad.
                order.emplace_back(RenderItem { *layer, nullptr });
            }
            continue;
        }

        if (layer->is<RenderCustomLayer>()) {
            order.emplace_back(RenderItem { *layer, nullptr });
            continue;
        }

        RenderSource* source = getRenderSource(layer->baseImpl->source);
        if (!source) {
            Log::Warning(Event::Render, "can't find source for layer '%s'", layer->getID().c_str());
            continue;
        }

        const bool symbolLayer = layer->is<RenderSymbolLayer>();

        auto sortedTiles = source->getRenderTiles();
        if (symbolLayer) {
            // Sort symbol tiles in opposite y position, so tiles with overlapping symbols are drawn
            // on top of each other, with lower symbols being drawn on top of higher symbols.
            std::sort(sortedTiles.begin(), sortedTiles.end(), [&](const RenderTile& a, const RenderTile& b) {
                Point<float> pa(a.id.canonical.x, a.id.canonical.y);
                Point<float> pb(b.id.canonical.x, b.id.canonical.y);

                auto par = util::rotate(pa, parameters.state.getAngle());
                auto pbr = util::rotate(pb, parameters.state.getAngle());

                return std::tie(par.y, par.x) < std::tie(pbr.y, pbr.x);
            });
        } else {
            std::sort(sortedTiles.begin(), sortedTiles.end(),
                      [](const auto& a, const auto& b) { return a.get().id < b.get().id; });
        }

        std::vector<std::reference_wrapper<RenderTile>> sortedTilesForInsertion;
        for (auto& sortedTile : sortedTiles) {
            auto& tile = sortedTile.get();
            if (!tile.tile.isRenderable()) {
                continue;
            }

            // We're not clipping symbol layers, so when we have both parents and children of symbol
            // layers, we drop all children in favor of their parent to avoid duplicate labels.
            // See https://github.com/mapbox/mapbox-gl-native/issues/2482
            if (symbolLayer) {
                bool skip = false;
                // Look back through the buckets we decided to render to find out whether there is
                // already a bucket from this layer that is a parent of this tile. Tiles are ordered
                // by zoom level when we obtain them from getTiles().
                for (auto it = sortedTilesForInsertion.rbegin();
                     it != sortedTilesForInsertion.rend(); ++it) {
                    if (tile.tile.id.isChildOf(it->get().tile.id)) {
                        skip = true;
                        break;
                    }
                }
                if (skip) {
                    continue;
                }
            }

            auto bucket = tile.tile.getBucket(*layer->baseImpl);
            if (bucket) {
                sortedTilesForInsertion.emplace_back(tile);
                tile.used = true;
            }
        }
        layer->setRenderTiles(std::move(sortedTilesForInsertion));
        order.emplace_back(RenderItem { *layer, source });
    }

    frameHistory.record(parameters.timePoint,
                        parameters.state.getZoom(),
                        parameters.mapMode == MapMode::Continuous ? util::DEFAULT_TRANSITION_DURATION : Milliseconds(0));

    // - UPLOAD PASS -------------------------------------------------------------------------------
    // Uploads all required buffers and images before we do any actual rendering.
    {
        MBGL_DEBUG_GROUP(parameters.context, "upload");

        parameters.imageManager.upload(parameters.context, 0);
        parameters.lineAtlas.upload(parameters.context, 0);
        parameters.frameHistory.upload(parameters.context, 0);
    }

    // - CLEAR -------------------------------------------------------------------------------------
    // Renders the backdrop of the OpenGL view. This also paints in areas where we don't have any
    // tiles whatsoever.
    {
        MBGL_DEBUG_GROUP(parameters.context, "clear");
        parameters.backend.bind();
        parameters.context.clear((parameters.debugOptions & MapDebugOptions::Overdraw)
                        ? Color::black()
                        : backgroundColor,
                      1.0f,
                      0);
    }

    // - CLIPPING MASKS ----------------------------------------------------------------------------
    // Draws the clipping masks to the stencil buffer.
    {
        MBGL_DEBUG_GROUP(parameters.context, "clip");

        // Update all clipping IDs.
        for (const auto& source : sources) {
            source->startRender(parameters);
        }

        MBGL_DEBUG_GROUP(parameters.context, "clipping masks");

        static const style::FillPaintProperties::PossiblyEvaluated properties {};
        static const FillProgram::PaintPropertyBinders paintAttibuteData(properties, 0);

        for (const auto& clipID : parameters.clipIDGenerator.getClipIDs()) {
            parameters.staticData.programs.fill.get(properties).draw(
                parameters.context,
                gl::Triangles(),
                gl::DepthMode::disabled(),
                gl::StencilMode {
                    gl::StencilMode::Always(),
                    static_cast<int32_t>(clipID.second.reference.to_ulong()),
                    0b11111111,
                    gl::StencilMode::Keep,
                    gl::StencilMode::Keep,
                    gl::StencilMode::Replace
                },
                gl::ColorMode::disabled(),
                FillProgram::UniformValues {
                    uniforms::u_matrix::Value{ parameters.matrixForTile(clipID.first) },
                    uniforms::u_world::Value{ parameters.context.viewport.getCurrentValue().size },
                },
                parameters.staticData.tileVertexBuffer,
                parameters.staticData.quadTriangleIndexBuffer,
                parameters.staticData.tileTriangleSegments,
                paintAttibuteData,
                properties,
                parameters.state.getZoom(),
                "clipping"
            );
        }
    }

#if not MBGL_USE_GLES2 and not defined(NDEBUG)
    // Render tile clip boundaries, using stencil buffer to calculate fill color.
    if (parameters.debugOptions & MapDebugOptions::StencilClip) {
        parameters.context.setStencilMode(gl::StencilMode::disabled());
        parameters.context.setDepthMode(gl::DepthMode::disabled());
        parameters.context.setColorMode(gl::ColorMode::unblended());
        parameters.context.program = 0;

        // Reset the value in case someone else changed it, or it's dirty.
        parameters.context.pixelTransferStencil = gl::value::PixelTransferStencil::Default;

        // Read the stencil buffer
        const auto viewport = parameters.context.viewport.getCurrentValue();
        auto image = parameters.context.readFramebuffer<AlphaImage, gl::TextureFormat::Stencil>(viewport.size, false);

        // Scale the Stencil buffer to cover the entire color space.
        auto it = image.data.get();
        auto end = it + viewport.size.width * viewport.size.height;
        const auto factor = 255.0f / *std::max_element(it, end);
        for (; it != end; ++it) {
            *it *= factor;
        }

        parameters.context.pixelZoom = { 1, 1 };
        parameters.context.rasterPos = { -1, -1, 0, 1 };
        parameters.context.drawPixels(image);

        return;
    }
#endif

    int indent = 0;

    // Actually render the layers
    if (debug::renderTree) { Log::Info(Event::Render, "{"); indent++; }

    parameters.depthRangeSize = 1 - (order.size() + 2) * parameters.numSublayers * parameters.depthEpsilon;

    // - OPAQUE PASS -------------------------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque objects first.
    {
        parameters.pass = RenderPass::Opaque;
        MBGL_DEBUG_GROUP(parameters.context, "opaque");

        if (debug::renderTree) {
            Log::Info(Event::Render, "%*s%s {", indent++ * 4, "", "opaque");
        }

        uint32_t i = 0;
        for (auto it = order.rbegin(); it != order.rend(); ++it, ++i) {
            parameters.currentLayer = i;
            if (it->layer.hasRenderPass(parameters.pass)) {
                MBGL_DEBUG_GROUP(parameters.context, it->layer.getID());
                it->layer.render(parameters, it->source);
            }
        }

        if (debug::renderTree) {
            Log::Info(Event::Render, "%*s%s", --indent * 4, "", "}");
        }
    }

    // - TRANSLUCENT PASS --------------------------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render bottom-to-top.
    {
        parameters.pass = RenderPass::Translucent;
        MBGL_DEBUG_GROUP(parameters.context, "translucent");

        if (debug::renderTree) {
            Log::Info(Event::Render, "%*s%s {", indent++ * 4, "", "translucent");
        }

        uint32_t i = static_cast<uint32_t>(order.size()) - 1;
        for (auto it = order.begin(); it != order.end(); ++it, --i) {
            parameters.currentLayer = i;
            if (it->layer.hasRenderPass(parameters.pass)) {
                MBGL_DEBUG_GROUP(parameters.context, it->layer.getID());
                it->layer.render(parameters, it->source);
            }
        }

        if (debug::renderTree) {
            Log::Info(Event::Render, "%*s%s", --indent * 4, "", "}");
        }
    }

    if (debug::renderTree) { Log::Info(Event::Render, "}"); indent--; }

    // - DEBUG PASS --------------------------------------------------------------------------------
    // Renders debug overlays.
    {
        MBGL_DEBUG_GROUP(parameters.context, "debug");

        // Finalize the rendering, e.g. by calling debug render calls per tile.
        // This guarantees that we have at least one function per tile called.
        // When only rendering layers via the stylesheet, it's possible that we don't
        // ever visit a tile during rendering.
        for (const auto& source : sources) {
            source->finishRender(parameters);
        }
    }

#if not MBGL_USE_GLES2 and not defined(NDEBUG)
    // Render the depth buffer.
    if (parameters.debugOptions & MapDebugOptions::DepthBuffer) {
        parameters.context.setStencilMode(gl::StencilMode::disabled());
        parameters.context.setDepthMode(gl::DepthMode::disabled());
        parameters.context.setColorMode(gl::ColorMode::unblended());
        parameters.context.program = 0;

        // Scales the values in the depth buffer so that they cover the entire grayscale range. This
        // makes it easier to spot tiny differences.
        const float base = 1.0f / (1.0f - parameters.depthRangeSize);
        parameters.context.pixelTransferDepth = { base, 1.0f - base };

        // Read the stencil buffer
        auto viewport = parameters.context.viewport.getCurrentValue();
        auto image = parameters.context.readFramebuffer<AlphaImage, gl::TextureFormat::Depth>(viewport.size, false);

        parameters.context.pixelZoom = { 1, 1 };
        parameters.context.rasterPos = { -1, -1, 0, 1 };
        parameters.context.drawPixels(image);
    }
#endif

    // TODO: Find a better way to unbind VAOs after we're done with them without introducing
    // unnecessary bind(0)/bind(N) sequences.
    {
        MBGL_DEBUG_GROUP(parameters.context, "cleanup");

        parameters.context.activeTexture = 1;
        parameters.context.texture[1] = 0;
        parameters.context.activeTexture = 0;
        parameters.context.texture[0] = 0;

        parameters.context.bindVertexArray = 0;
    }

    observer->onDidFinishRenderingFrame(
        loaded ? RendererObserver::RenderMode::Full : RendererObserver::RenderMode::Partial,
        updateParameters.mode == MapMode::Continuous && (hasTransitions() || frameHistory.needsAnimation(util::DEFAULT_TRANSITION_DURATION))
    );

    if (!loaded) {
        renderState = RenderState::Partial;
    } else if (renderState != RenderState::Fully) {
        renderState = RenderState::Fully;
        observer->onDidFinishRenderingMap();
    }

    // Cleanup only after signaling completion
    parameters.context.performCleanup();
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

    std::unordered_set<std::string> sourceIDs;
    for (const RenderLayer* layer : layers) {
        sourceIDs.emplace(layer->baseImpl->source);
    }

    std::unordered_map<std::string, std::vector<Feature>> resultsByLayer;
    for (const auto& sourceID : sourceIDs) {
        if (RenderSource* renderSource = getRenderSource(sourceID)) {
            auto sourceResults = renderSource->queryRenderedFeatures(geometry, transformState, layers, options);
            std::move(sourceResults.begin(), sourceResults.end(), std::inserter(resultsByLayer, resultsByLayer.begin()));
        }
    }

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

std::vector<Feature> Renderer::Impl::querySourceFeatures(const std::string& sourceID, const SourceQueryOptions& options) const {
    const RenderSource* source = getRenderSource(sourceID);
    if (!source) return {};

    return source->querySourceFeatures(options);
}

void Renderer::Impl::onLowMemory() {
    assert(BackendScope::exists());
    backend.getContext().performCleanup();
    for (const auto& entry : renderSources) {
        entry.second->onLowMemory();
    }
    observer->onInvalidate();
}

void Renderer::Impl::dumDebugLogs() {
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

bool Renderer::Impl::hasTransitions() const {
    if (renderLight.hasTransition()) {
        return true;
    }

    for (const auto& entry : renderLayers) {
        if (entry.second->hasTransition()) {
            return true;
        }
    }

    return false;
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

} // namespace mbgl
