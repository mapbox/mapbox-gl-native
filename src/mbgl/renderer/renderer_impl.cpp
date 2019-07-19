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

Renderer::Impl::Impl(gfx::RendererBackend& backend_,
                     float pixelRatio_,
                     optional<std::string> programCacheDir_,
                     optional<std::string> localFontFamily_)
    : orchestrator(!backend_.contextIsShared(), std::move(localFontFamily_))
    , backend(backend_)
    , observer(&nullObserver())
    , pixelRatio(pixelRatio_)
    , programCacheDir(std::move(programCacheDir_)) {

}

Renderer::Impl::~Impl() {
    assert(gfx::BackendScope::exists());
};

void Renderer::Impl::setObserver(RendererObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver();
}

void Renderer::Impl::render(const RenderTree& renderTree) {
    if (renderState == RenderState::Never) {
        observer->onWillStartRenderingMap();
    }

    observer->onWillStartRenderingFrame();
    const auto& renderTreeParameters = renderTree.getParameters();

    if (!staticData) {
        staticData = std::make_unique<RenderStaticData>(backend.getContext(), pixelRatio, programCacheDir);
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
        renderTreeParameters.needsRepaint
    );

    if (!renderTreeParameters.loaded) {
        renderState = RenderState::Partial;
    } else if (renderState != RenderState::Fully) {
        renderState = RenderState::Fully;
        observer->onDidFinishRenderingMap();
    }
}

void Renderer::Impl::reduceMemoryUse() {
    assert(gfx::BackendScope::exists());
    backend.getContext().reduceMemoryUsage();
}

} // namespace mbgl
