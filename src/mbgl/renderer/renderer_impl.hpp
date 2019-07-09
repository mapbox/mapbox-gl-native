#pragma once

#include <mbgl/renderer/render_orchestrator.hpp>

#include <memory>
#include <string>

namespace mbgl {

class RendererObserver;
class RenderStaticData;
class RenderTree;

namespace gfx {
class RendererBackend;
} // namespace gfx

class Renderer::Impl {
public:
    Impl(gfx::RendererBackend&,
         float pixelRatio_,
         optional<std::string> programCacheDir,
         optional<std::string> localFontFamily_);
    ~Impl();

private:
    friend class Renderer;

    void setObserver(RendererObserver*);

    void render(const RenderTree&);

    void reduceMemoryUse();

    // TODO: Move orchestrator to Map::Impl.
    RenderOrchestrator orchestrator;

    gfx::RendererBackend& backend;

    RendererObserver* observer;

    const float pixelRatio;
    const optional<std::string> programCacheDir;
    std::unique_ptr<RenderStaticData> staticData;

    enum class RenderState {
        Never,
        Partial,
        Fully,
    };

    RenderState renderState = RenderState::Never;
};

} // namespace mbgl
