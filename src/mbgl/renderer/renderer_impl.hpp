#pragma once
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/renderer/render_style_observer.hpp>
#include <mbgl/style/style.hpp>

#include <mbgl/map/backend.hpp>

#include <memory>
#include <string>

namespace mbgl {

enum class RenderState : uint8_t {
    Never,
    Partial,
    Fully,
};

class Painter;
class RenderStyle;
class TransformState;
class View;


class Renderer::Impl : public RenderStyleObserver {
public:
    Impl(Backend&, float pixelRatio_, FileSource&, Scheduler&, GLContextMode,
         const optional<std::string> programCacheDir);
    ~Impl() final;

    void setObserver(RendererObserver*);

    void render(View&, const UpdateParameters&);

    std::vector<Feature> queryRenderedFeatures(const ScreenLineString&, const RenderedQueryOptions&) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions&) const;

    void onLowMemory();

    void dumDebugLogs() ;

    // RenderStyleObserver implementation
    void onInvalidate()override;
    void onResourceError(std::exception_ptr) override;

private:
    friend class Renderer;

    Backend& backend;

    RendererObserver* observer;

    const GLContextMode contextMode;
    const float pixelRatio;
    const optional<std::string> programCacheDir;

    RenderState renderState = RenderState::Never;

    std::unique_ptr<RenderStyle> renderStyle;
    std::unique_ptr<Painter> painter;

};

} // namespace mbgl
