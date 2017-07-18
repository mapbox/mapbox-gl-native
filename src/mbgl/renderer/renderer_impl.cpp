#include <mbgl/renderer/renderer_impl.hpp>
#include <mbgl/renderer/render_style.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/renderer/render_item.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/gl/debugging.hpp>
#include <mbgl/geometry/line_atlas.hpp>

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
        , observer(&nullObserver())
        , contextMode(contextMode_)
        , pixelRatio(pixelRatio_)
        , programCacheDir(programCacheDir_)
        , renderStyle(std::make_unique<RenderStyle>(scheduler_, fileSource_)) {

    renderStyle->setObserver(this);
}

Renderer::Impl::~Impl() {
    BackendScope guard { backend, backend.getScopeType()};
    renderStyle.reset();
    staticData.reset();
};

void Renderer::Impl::setObserver(RendererObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver();
}

void Renderer::Impl::render(View& view, const UpdateParameters& updateParameters) {
    // Don't load/render anyting in still mode until explicitly requested.
    if (updateParameters.mode == MapMode::Still && !updateParameters.stillImageRequest) return;

    BackendScope guard { backend, backend.getScopeType() };

    renderStyle->update(updateParameters);
    transformState = updateParameters.transformState;

    if (!staticData) {
        staticData = std::make_unique<RenderStaticData>(backend.getContext(), pixelRatio, programCacheDir);
    }

    PaintParameters parameters {
        backend.getContext(),
        pixelRatio,
        contextMode,
        view,
        updateParameters,
        *renderStyle,
        *staticData,
        frameHistory
    };

    bool loaded = updateParameters.styleLoaded && renderStyle->isLoaded();

    if (updateParameters.mode == MapMode::Continuous) {
        if (renderState == RenderState::Never) {
            observer->onWillStartRenderingMap();
        }

        observer->onWillStartRenderingFrame();

        backend.updateAssumedState();

        doRender(parameters);
        parameters.context.performCleanup();

        observer->onDidFinishRenderingFrame(
                loaded ? RendererObserver::RenderMode::Full : RendererObserver::RenderMode::Partial,
                renderStyle->hasTransitions() || frameHistory.needsAnimation(util::DEFAULT_TRANSITION_DURATION)
        );

        if (!loaded) {
            renderState = RenderState::Partial;
        } else if (renderState != RenderState::Fully) {
            renderState = RenderState::Fully;
            observer->onDidFinishRenderingMap();
        }
    } else if (loaded) {
        observer->onWillStartRenderingMap();
        observer->onWillStartRenderingFrame();

        backend.updateAssumedState();

        doRender(parameters);

        observer->onDidFinishRenderingFrame(RendererObserver::RenderMode::Full, false);
        observer->onDidFinishRenderingMap();

        // Cleanup only after signaling completion
        parameters.context.performCleanup();
    }
}

void Renderer::Impl::doRender(PaintParameters& parameters) {
    if (parameters.contextMode == GLContextMode::Shared) {
        parameters.context.setDirtyState();
    }

    RenderData renderData = renderStyle->getRenderData(parameters.debugOptions, parameters.state.getAngle());
    const std::vector<RenderItem>& order = renderData.order;
    const std::unordered_set<RenderSource*>& sources = renderData.sources;

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
        parameters.view.bind();
        parameters.context.clear((parameters.debugOptions & MapDebugOptions::Overdraw)
                        ? Color::black()
                        : renderData.backgroundColor,
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
}

std::vector<Feature> Renderer::Impl::queryRenderedFeatures(const ScreenLineString& geometry, const RenderedQueryOptions& options) const {
    return renderStyle->queryRenderedFeatures(geometry, transformState, options);
}

std::vector<Feature> Renderer::Impl::querySourceFeatures(const std::string& sourceID, const SourceQueryOptions& options) const {
    const RenderSource* source = renderStyle->getRenderSource(sourceID);
    if (!source) return {};

    return source->querySourceFeatures(options);
}

void Renderer::Impl::onInvalidate() {
    observer->onInvalidate();
}

void Renderer::Impl::onResourceError(std::exception_ptr ptr) {
    observer->onResourceError(ptr);
}

void Renderer::Impl::onLowMemory() {
    BackendScope guard { backend, backend.getScopeType() };
    backend.getContext().performCleanup();
    renderStyle->onLowMemory();
    observer->onInvalidate();
}

void Renderer::Impl::dumDebugLogs() {
    renderStyle->dumpDebugLogs();
}

} // namespace mbgl
