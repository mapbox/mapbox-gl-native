
#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>

#include <functional>
#include <memory>
#include <vector>
#include <string>

namespace mbgl {

class Renderer;

class FileSource;
class Scheduler;
class RenderedQueryOptions;
class SourceQueryOptions;

namespace util {
template <class> class Thread;
} // namespace util

namespace android {

class AndroidRendererBackend;

class AndroidRendererFrontend : public RendererFrontend {
public:
    using InvalidateCallback = std::function<void ()>;

    AndroidRendererFrontend(float pixelRatio,
                            mbgl::FileSource&,
                            mbgl::Scheduler&,
                            std::string programCacheDir,
                            InvalidateCallback);
    ~AndroidRendererFrontend() override;

    void reset() override;
    void setObserver(RendererObserver&) override;

    void update(std::shared_ptr<UpdateParameters>) override;

    // Called from OpenGL Thread
    void render();

    // Feature querying
    std::vector<Feature> queryRenderedFeatures(const ScreenCoordinate&, const RenderedQueryOptions&) const;
    std::vector<Feature> queryRenderedFeatures(const ScreenBox&, const RenderedQueryOptions&) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions&) const;
    AnnotationIDs queryPointAnnotations(const ScreenBox& box) const;

    // RenderBackend proxy - Called from OpenGL Thread
    void resizeFramebuffer(int width, int height);

    // Memory
    void onLowMemory();

    // Snapshot - Callback will be called on calling thread through RunLoop
    using SnapshotCallback = std::function<void (PremultipliedImage)>;
    void requestSnapshot(SnapshotCallback);

private:
    std::unique_ptr<AndroidRendererBackend> backend;
    std::unique_ptr<util::Thread<Renderer>> renderer;
    std::unique_ptr<RendererObserver> rendererObserver;
    std::shared_ptr<UpdateParameters> updateParameters;
    util::AsyncTask asyncInvalidate;

    util::RunLoop* mapRunLoop;

    bool framebufferSizeChanged = true;
    std::unique_ptr<SnapshotCallback> snapshotCallback;
};

} // namespace android
} // namespace mbgl
