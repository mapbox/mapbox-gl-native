#include "glfw_renderer_frontend.hpp"

#include <mbgl/renderer/renderer.hpp>

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
    
    mbgl::BackendScope guard { glfwView, mbgl::BackendScope::ScopeType::Implicit };

    renderer->render(*updateParameters);
}

mbgl::Renderer* GLFWRendererFrontend::getRenderer() {
    assert(renderer);
    return renderer.get();
}
