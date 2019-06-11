#include <mbgl/renderer/renderer_impl.hpp>

#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/gfx/upload_pass.hpp>
#include <mbgl/gfx/render_pass.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/renderer/pattern_atlas.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/renderer/render_tree.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

using namespace style;

static RendererObserver& nullObserver() {
    static RendererObserver observer;
    return observer;
}

namespace {

struct {
    double totalTime = 0.0;
    double orchestrationTime = 0.0;
    double orchestrationTimePeak = 0.0;
    double orchestrationPercentagePeak = 0.0;
    double uploadTime = 0.0;
    double uploadTimePeak = 0.0;
    double uploadPercentagePeak = 0.0;
    std::size_t failedFrames = 0u;
    std::size_t totalFrames = 0u;
    std::size_t framesSavedWithBackgroundOrchestration = 0u;
} stats;

} // namespace

Renderer::Impl::Impl(gfx::RendererBackend& backend_,
                     float pixelRatio_,
                     optional<std::string> localFontFamily_)
    : orchestrator(!backend_.contextIsShared(), std::move(localFontFamily_))
    , backend(backend_)
    , observer(&nullObserver())
    , pixelRatio(pixelRatio_) {

}

Renderer::Impl::~Impl() {
    Log::Warning(Event::General,
                 "Overall stats: orchestration took %f ms from %f ms of the total rendering time (%f%%)",
                 stats.orchestrationTime,
                 stats.totalTime,
                 stats.orchestrationTime / stats.totalTime * 100.0);
    Log::Warning(Event::General,
                 "Overall stats: orchestration peak value: %f ms, orchestration peak frame time percentage: %f",
                 stats.orchestrationTimePeak,
                 stats.orchestrationPercentagePeak);
    Log::Warning(Event::General,
                 "Overall stats: upload peak value: %f ms, upload peak frame time percentage: %f",
                 stats.uploadTimePeak,
                 stats.uploadPercentagePeak);
    Log::Warning(Event::General,
                 "Overall stats: %zu frames out of %zu took > 16.7 ms (%f%%), %zu of them took too long because of "
                 "orchestration (%f%%)",
                 stats.failedFrames,
                 stats.totalFrames,
                 double(stats.failedFrames) / stats.totalFrames * 100.0,
                 stats.framesSavedWithBackgroundOrchestration,
                 double(stats.framesSavedWithBackgroundOrchestration) / stats.failedFrames * 100.0);
    assert(gfx::BackendScope::exists());
};

void Renderer::Impl::setObserver(RendererObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver();
}

void Renderer::Impl::render(const RenderTree& renderTree, TimePoint frame_start) {
    auto render_start = Clock::now();
    if (renderState == RenderState::Never) {
        observer->onWillStartRenderingMap();
    }

    observer->onWillStartRenderingFrame();
    const auto& renderTreeParameters = renderTree.getParameters();

    if (!staticData) {
        staticData = std::make_unique<RenderStaticData>(backend.getContext(), pixelRatio);
    }
    staticData->has3D = renderTreeParameters.has3D;

    auto& context = backend.getContext();

    // Blocks execution until the renderable is available.
    backend.getDefaultRenderable().wait();

    PaintParameters parameters {
        context,
        pixelRatio,
        backend,
        renderTreeParameters.light,
        renderTreeParameters.mapMode,
        renderTreeParameters.debugOptions,
        renderTreeParameters.timePoint,
        renderTreeParameters.transformParams,
        *staticData,
        renderTree.getLineAtlas(),
        renderTree.getPatternAtlas()
    };

    parameters.symbolFadeChange = renderTreeParameters.symbolFadeChange;
    parameters.opaquePassCutoff = renderTreeParameters.opaquePassCutOff;
    const auto& sourceRenderItems = renderTree.getSourceRenderItems();
    const auto& layerRenderItems = renderTree.getLayerRenderItems();

    auto upload_start = Clock::now();

    // - UPLOAD PASS -------------------------------------------------------------------------------
    // Uploads all required buffers and images before we do any actual rendering.
    {
        const auto uploadPass = parameters.encoder->createUploadPass("upload");

        // Update all clipping IDs + upload buckets.
        for (const RenderItem& item : sourceRenderItems) {
            item.upload(*uploadPass);
        }
        for (const RenderItem& item : layerRenderItems) {
            item.upload(*uploadPass);
        }
        staticData->upload(*uploadPass);
        renderTree.getLineAtlas().upload(*uploadPass);
        renderTree.getPatternAtlas().upload(*uploadPass);
    }
    auto upload_end = Clock::now();

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

        uint32_t i = static_cast<uint32_t>(layerRenderItems.size()) - 1;
        for (auto it = layerRenderItems.begin(); it != layerRenderItems.end(); ++it, --i) {
            parameters.currentLayer = i;
            const RenderItem& renderItem = it->get();
            if (renderItem.hasRenderPass(parameters.pass)) {
                const auto layerDebugGroup(parameters.encoder->createDebugGroup(renderItem.getName().c_str()));
                renderItem.render(parameters);
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
            color = renderTreeParameters.backgroundColor;
        }
        parameters.renderPass = parameters.encoder->createRenderPass("main buffer", { parameters.backend.getDefaultRenderable(), color, 1, 0 });
    }

    // Actually render the layers

    parameters.depthRangeSize = 1 - (layerRenderItems.size() + 2) * parameters.numSublayers * parameters.depthEpsilon;

    // - OPAQUE PASS -------------------------------------------------------------------------------
    // Render everything top-to-bottom by using reverse iterators. Render opaque objects first.
    {
        parameters.pass = RenderPass::Opaque;
        const auto debugGroup(parameters.renderPass->createDebugGroup("opaque"));

        uint32_t i = 0;
        for (auto it = layerRenderItems.rbegin(); it != layerRenderItems.rend(); ++it, ++i) {
            parameters.currentLayer = i;
            const RenderItem& renderItem = it->get();
            if (renderItem.hasRenderPass(parameters.pass)) {
                const auto layerDebugGroup(parameters.renderPass->createDebugGroup(renderItem.getName().c_str()));
                renderItem.render(parameters);
            }
        }
    }

    // - TRANSLUCENT PASS --------------------------------------------------------------------------
    // Make a second pass, rendering translucent objects. This time, we render bottom-to-top.
    {
        parameters.pass = RenderPass::Translucent;
        const auto debugGroup(parameters.renderPass->createDebugGroup("translucent"));

        uint32_t i = static_cast<uint32_t>(layerRenderItems.size()) - 1;
        for (auto it = layerRenderItems.begin(); it != layerRenderItems.end(); ++it, --i) {
            parameters.currentLayer = i;
            const RenderItem& renderItem = it->get();
            if (renderItem.hasRenderPass(parameters.pass)) {
                const auto layerDebugGroup(parameters.renderPass->createDebugGroup(renderItem.getName().c_str()));
                renderItem.render(parameters);
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
        for (const RenderItem& renderItem : sourceRenderItems) {           
            renderItem.render(parameters);
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

    // Ends the RenderPass
    parameters.renderPass.reset();
    const bool isMapModeContinuous = renderTreeParameters.mapMode == MapMode::Continuous;
    if (isMapModeContinuous) {
        parameters.encoder->present(parameters.backend.getDefaultRenderable());
    }

    // CommandEncoder destructor submits render commands.
    parameters.encoder.reset();

    observer->onDidFinishRenderingFrame(
        renderTreeParameters.loaded ? RendererObserver::RenderMode::Full : RendererObserver::RenderMode::Partial,
        renderTreeParameters.needsRepaint,
        renderTreeParameters.placementChanged
    );

    if (!renderTreeParameters.loaded) {
        renderState = RenderState::Partial;
    } else if (renderState != RenderState::Fully) {
        renderState = RenderState::Fully;
        observer->onDidFinishRenderingMap();
    }

    auto frame_end = Clock::now();
    std::chrono::duration<double, std::milli> frameDuration = frame_end - frame_start;
    std::chrono::duration<double, std::milli> orchDuration = render_start - frame_start;
    std::chrono::duration<double, std::milli> renderDuration = frame_end - render_start;
    std::chrono::duration<double, std::milli> uploadDuration = upload_end - upload_start;

    stats.totalTime += frameDuration.count();
    stats.orchestrationTime += orchDuration.count();

    double orchestrationPercentage = orchDuration.count() / frameDuration.count() * 100.0;
    double uploadPercentage = uploadDuration.count() / frameDuration.count() * 100.0;

    if (stats.orchestrationTimePeak < orchDuration.count()) {
        stats.orchestrationTimePeak = orchDuration.count();
    }
    if (stats.orchestrationPercentagePeak < orchestrationPercentage) {
        stats.orchestrationPercentagePeak = orchestrationPercentage;
    }
    if (stats.uploadTimePeak < uploadDuration.count()) {
        stats.uploadTimePeak = uploadDuration.count();
    }
    if (stats.uploadPercentagePeak < uploadPercentage) {
        stats.uploadPercentagePeak = uploadPercentage;
    }

    constexpr double kMaxFrameDuration = 16.7; // 60 fps
    stats.totalFrames++;
    if (frameDuration.count() > kMaxFrameDuration) {
        stats.failedFrames++;
        Log::Warning(Event::General,
                     "Failed frame stats: total: %f ms, orchestration %f ms, rendering %f ms, upload %f ms",
                     frameDuration.count(),
                     orchDuration.count(),
                     renderDuration.count(),
                     uploadDuration.count());
        if (frameDuration.count() - orchDuration.count() <= kMaxFrameDuration) {
            stats.framesSavedWithBackgroundOrchestration++;
        }
    }
}

void Renderer::Impl::reduceMemoryUse() {
    assert(gfx::BackendScope::exists());
    backend.getContext().reduceMemoryUsage();
}

} // namespace mbgl
