#include "glfw_renderer_frontend.hpp"

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/perf/runtime_metrics.hpp>

GLFWRendererFrontend::GLFWRendererFrontend(std::unique_ptr<mbgl::Renderer> renderer_, GLFWView& glfwView_)
    : glfwView(glfwView_)
    , renderer(std::move(renderer_)) {
        glfwView.setRenderFrontend(this);
}

GLFWRendererFrontend::~GLFWRendererFrontend() = default;

void GLFWRendererFrontend::reset() {
    assert(renderer);
    renderer.reset();
}

void GLFWRendererFrontend::setObserver(mbgl::RendererObserver& observer) {
    assert(renderer);
    renderer->setObserver(&observer);
}

void GLFWRendererFrontend::update(std::shared_ptr<mbgl::UpdateParameters> params) {
    updateParameters = std::move(params);
    glfwView.invalidate();
}

void GLFWRendererFrontend::render() {
    assert(renderer);
    
    if (!updateParameters) return;
    MBGL_TRACE_RENDERER_BEGINRECORD(update);
    MBGL_TRACE_RENDERER_BEGIN(attach);
    mbgl::gfx::BackendScope guard { glfwView.getRendererBackend(), mbgl::gfx::BackendScope::ScopeType::Implicit };

    // onStyleImageMissing might be called during a render. The user implemented method
    // could trigger a call to MGLRenderFrontend#update which overwrites `updateParameters`.
    // Copy the shared pointer here so that the parameters aren't destroyed while `render(...)` is
    // still using them.
    auto updateParameters_ = updateParameters;
    MBGL_TRACE_RENDERER_END(attach);
    renderer->render(updateParameters_);
    MBGL_TRACE_RENDERER_ENDRECORD(update);
}

mbgl::Renderer* GLFWRendererFrontend::getRenderer() {
    assert(renderer);
    return renderer.get();
}

void GLFWRendererFrontend::setRecordFrame(bool enabled)
{
    recordFrames = enabled;
}
