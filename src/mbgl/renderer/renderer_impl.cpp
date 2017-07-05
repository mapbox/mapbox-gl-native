#include <mbgl/renderer/renderer_impl.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/renderer/render_style.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/renderer/backend_scope.hpp>

namespace mbgl {

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
        , pixelRatio(pixelRatio_),
          programCacheDir(programCacheDir_)
        , renderStyle(std::make_unique<RenderStyle>(scheduler_, fileSource_)) {

    renderStyle->setObserver(this);
}

Renderer::Impl::~Impl() {
    BackendScope guard { backend, backend.getScopeType()};
    renderStyle.reset();
};

void Renderer::Impl::setObserver(RendererObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver();
}

void Renderer::Impl::render(View& view, const UpdateParameters& updateParameters) {
    // Don't load/render anyting in still mode until explicitly requested.
    if (updateParameters.mode == MapMode::Still && !updateParameters.stillImageRequest) return;

    BackendScope guard { backend, backend.getScopeType() };

    // Update render style
    renderStyle->update(updateParameters);

    // Initialize painter
    if (!painter) {
        gl::Context& context = backend.getContext();
        painter = std::make_unique<Painter>(context,
                                            pixelRatio,
                                            programCacheDir);
    }

    // Update transform state on painter.
    painter->state = updateParameters.transformState;

    bool loaded = updateParameters.styleLoaded && renderStyle->isLoaded();

    if (updateParameters.mode == MapMode::Continuous) {
        if (renderState == RenderState::Never) {
            observer->onWillStartRenderingMap();
        }

        observer->onWillStartRenderingFrame();

        FrameData frameData { updateParameters.timePoint,
                              pixelRatio,
                              updateParameters.mode,
                              contextMode,
                              updateParameters.debugOptions };

        backend.updateAssumedState();

        painter->render(*renderStyle, frameData, view);
        painter->cleanup();

        observer->onDidFinishRenderingFrame(
                loaded ? RendererObserver::RenderMode::Full : RendererObserver::RenderMode::Partial,
                renderStyle->hasTransitions() || painter->needsAnimation()
        );

        if (!loaded) {
            renderState = RenderState::Partial;
        } else if (renderState != RenderState::Fully) {
            renderState = RenderState::Fully;
            observer->onDidFinishRenderingMap();
        }
    } else if (loaded) {
        // We can render the map in still mode
        observer->onWillStartRenderingMap();
        observer->onWillStartRenderingFrame();

        FrameData frameData { updateParameters.timePoint,
                              pixelRatio,
                              updateParameters.mode,
                              contextMode,
                              updateParameters.debugOptions };

        backend.updateAssumedState();

        painter->render(*renderStyle, frameData, view);

        observer->onDidFinishRenderingFrame(RendererObserver::RenderMode::Full, false);
        observer->onDidFinishRenderingMap();
        
        // Cleanup only after signaling completion
        painter->cleanup();
    }
}

std::vector<Feature> Renderer::Impl::queryRenderedFeatures(const ScreenLineString& geometry, const RenderedQueryOptions& options) const {
    if (!painter) return {};

    return renderStyle->queryRenderedFeatures(geometry, painter->state, options);
}

std::vector<Feature> Renderer::Impl::querySourceFeatures(const std::string& sourceID, const SourceQueryOptions& options) const {
    const RenderSource* source = renderStyle->getRenderSource(sourceID);
    if (!source) return {};

    return source->querySourceFeatures(options);
}

void Renderer::Impl::onInvalidate() {
    observer->onInvalidate();
};

void Renderer::Impl::onResourceError(std::exception_ptr ptr) {
    observer->onResourceError(ptr);
}

void Renderer::Impl::onLowMemory() {
    if (painter) {
        BackendScope { backend, backend.getScopeType() };
        painter->cleanup();
    }
    renderStyle->onLowMemory();
    observer->onInvalidate();
}

void Renderer::Impl::dumDebugLogs() {
    renderStyle->dumpDebugLogs();
};


}
