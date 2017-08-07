#pragma once

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/renderer/render_style_observer.hpp>
#include <mbgl/renderer/frame_history.hpp>
#include <mbgl/map/transform_state.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class UpdateParameters;
class PaintParameters;
class RenderStyle;
class RenderStaticData;

class Renderer::Impl : public RenderStyleObserver {
public:
    Impl(RendererBackend&, float pixelRatio_, FileSource&, Scheduler&, GLContextMode,
         const optional<std::string> programCacheDir);
    ~Impl() final;

    void setObserver(RendererObserver*);

    void render(const UpdateParameters&);

    std::vector<Feature> queryRenderedFeatures(const ScreenLineString&, const RenderedQueryOptions&) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions&) const;

    void onLowMemory();
    void dumDebugLogs();

    // RenderStyleObserver implementation
    void onInvalidate() override;
    void onResourceError(std::exception_ptr) override;

private:
    void doRender(PaintParameters&);

    friend class Renderer;

    RendererBackend& backend;
    RendererObserver* observer;

    const GLContextMode contextMode;
    const float pixelRatio;
    const optional<std::string> programCacheDir;

    enum class RenderState {
        Never,
        Partial,
        Fully,
    };

    RenderState renderState = RenderState::Never;
    FrameHistory frameHistory;
    TransformState transformState;

    std::unique_ptr<RenderStyle> renderStyle;
    std::unique_ptr<RenderStaticData> staticData;
};

} // namespace mbgl
