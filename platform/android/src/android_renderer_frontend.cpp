#include "android_renderer_frontend.hpp"

#include <mbgl/actor/scheduler.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/run_loop.hpp>

#include "android_renderer_backend.hpp"

namespace mbgl {
namespace android {

// Forwards RendererObserver signals to the given
// Delegate RendererObserver on the given RunLoop
class ForwardingRendererObserver : public RendererObserver {
public:
    ForwardingRendererObserver(util::RunLoop& mapRunLoop_, RendererObserver& delegate_)
            : mapRunLoop(mapRunLoop_)
            , delegate(delegate_) {}

    void onInvalidate() override {
        mapRunLoop.invoke([&]() {
            delegate.onInvalidate();
        });
    }

    void onResourceError(std::exception_ptr err) override {
        mapRunLoop.invoke([&, err]() {
            delegate.onResourceError(err);
        });
    }

    void onWillStartRenderingMap() override {
        mapRunLoop.invoke([&]() {
            delegate.onWillStartRenderingMap();
        });
    }

    void onWillStartRenderingFrame() override {
        mapRunLoop.invoke([&]() {
            delegate.onWillStartRenderingFrame();
        });
    }

    void onDidFinishRenderingFrame(RenderMode mode, bool repaintNeeded) override {
        mapRunLoop.invoke([&, mode, repaintNeeded]() {
            delegate.onDidFinishRenderingFrame(mode, repaintNeeded);
        });
    }

    void onDidFinishRenderingMap() override {
        mapRunLoop.invoke([&]() {
            delegate.onDidFinishRenderingMap();
        });
    }

private:
    util::RunLoop& mapRunLoop;
    RendererObserver& delegate;
};

AndroidRendererFrontend::AndroidRendererFrontend(float pixelRatio_,
                                                 FileSource& fileSource_,
                                                 Scheduler& scheduler_,
                                                 std::string programCacheDir_,
                                                 InvalidateCallback invalidate_)
        : pixelRatio(pixelRatio_)
        , fileSource(fileSource_)
        , scheduler(scheduler_)
        , programCacheDir(programCacheDir_)
        , asyncInvalidate([=, invalidate=std::move(invalidate_)]() {
            invalidate();
        })
        , mapRunLoop(util::RunLoop::Get()) {
}

AndroidRendererFrontend::~AndroidRendererFrontend() = default;

void AndroidRendererFrontend::reset() {
    assert(renderer);
    renderer.reset();
}

void AndroidRendererFrontend::initialise() {
    // Lock as the initialization can come from the main thread or the GL thread first
    std::lock_guard<std::mutex> lock(intitialisationMutex);

    if (backend) {
        // The android system will have already destroyed the underlying
        // GL resources and an attempt to clean them up will fail
        backend->abandonContext();
    }

    // Destroy in reverse order
    renderer.reset();
    backend.reset();

    // Re-create
    backend = std::make_unique<AndroidRendererBackend>();
    renderer = std::make_unique<util::Thread<Renderer>>(
            "Orchestration Thread",
            *backend,
            pixelRatio,
            fileSource,
            scheduler,
            GLContextMode::Unique,
            programCacheDir
    );

    // Set the observer on the new Renderer implementation
    if (rendererObserver) {
        renderer->actor().invoke(&Renderer::setObserver, rendererObserver.get());
    }
}

void AndroidRendererFrontend::setObserver(RendererObserver& observer) {
    assert (util::RunLoop::Get());

    // Lock as the initialization can come from the main thread or the GL thread first
    std::lock_guard<std::mutex> lock(intitialisationMutex);
    
    rendererObserver = std::make_unique<ForwardingRendererObserver>(*mapRunLoop, observer);

    // Set the new observer on the Renderer implementation
    if (renderer) {
        renderer->actor().invoke(&Renderer::setObserver, rendererObserver.get());
    }
}

void AndroidRendererFrontend::update(std::shared_ptr<UpdateParameters> params) {
    {
        // Lock on the parameters
        std::lock_guard<std::mutex> lock(updateMutex);
        updateParameters = std::move(params);
    }
    asyncInvalidate.send();
}

// Called on OpenGL thread
void AndroidRendererFrontend::render() {
    assert (renderer);

    std::shared_ptr<UpdateParameters> params;
    {
        // Lock on the parameters
        std::unique_lock<std::mutex> lock(updateMutex);
        if (!updateParameters) return;

        // Hold on to the update parameters during render
        params = updateParameters;
    }

    // Activate the backend
    BackendScope backendGuard { *backend };

    // Block the orchestration thread during render
    util::BlockingThreadGuard<Renderer> rendererGuard { *renderer };

    // Ensure that the "current" scheduler on the render thread is
    // actually the scheduler from the orchestration  thread
    Scheduler::SetCurrent(renderer.get());

    if (framebufferSizeChanged) {
        backend->updateViewPort();
        framebufferSizeChanged = false;
    }

    rendererGuard.object().render(*params);
}

void AndroidRendererFrontend::onLowMemory() {
    assert (renderer);
    renderer->actor().invoke(&Renderer::onLowMemory);
}

std::vector<Feature> AndroidRendererFrontend::querySourceFeatures(const std::string& sourceID,
                                                                  const SourceQueryOptions& options) const {
    assert (renderer);
    // Waits for the result from the orchestration thread and returns
    return renderer->actor().ask(&Renderer::querySourceFeatures, sourceID, options).get();
}

std::vector<Feature> AndroidRendererFrontend::queryRenderedFeatures(const ScreenBox& box,
                                                                    const RenderedQueryOptions& options) const {
    assert (renderer);

    // Select the right overloaded method
    std::vector<Feature> (Renderer::*fn)(const ScreenBox&, const RenderedQueryOptions&) const = &Renderer::queryRenderedFeatures;

    // Waits for the result from the orchestration thread and returns
    return renderer->actor().ask(fn, box, options).get();
}

std::vector<Feature> AndroidRendererFrontend::queryRenderedFeatures(const ScreenCoordinate& point,
                                                                    const RenderedQueryOptions& options) const {
    assert (renderer);

    // Select the right overloaded method
    std::vector<Feature> (Renderer::*fn)(const ScreenCoordinate&, const RenderedQueryOptions&) const = &Renderer::queryRenderedFeatures;

    // Waits for the result from the orchestration thread and returns
    return renderer->actor().ask(fn, point, options).get();
}

AnnotationIDs AndroidRendererFrontend::queryPointAnnotations(const ScreenBox& box) const {
    assert (renderer);

    // Waits for the result from the orchestration thread and returns
    return renderer->actor().ask(&Renderer::queryPointAnnotations, box).get();
}

void AndroidRendererFrontend::requestSnapshot(SnapshotCallback callback_) {
    snapshotCallback = std::make_unique<SnapshotCallback>([callback=std::move(callback_), runloop=util::RunLoop::Get()](PremultipliedImage image){
        runloop->invoke([&]() {
            callback(std::move(image));
        });
    });
}

void AndroidRendererFrontend::resizeFramebuffer(int width, int height) {
    util::BlockingThreadGuard<Renderer> guard { *renderer };
    backend->resizeFramebuffer(width, height);
    framebufferSizeChanged = true;
    // TODO: thread safe?
    asyncInvalidate.send();
}

} // namespace android
} // namespace mbgl

