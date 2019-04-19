#include "glfw_gl_backend.hpp"

#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gl/renderable_resource.hpp>

#include <GLFW/glfw3.h>

class GLFWGLRenderableResource final : public mbgl::gl::RenderableResource {
public:
    GLFWGLRenderableResource(GLFWGLBackend& backend_) : backend(backend_) {
    }

    void bind() override {
        backend.setFramebufferBinding(0);
        backend.setViewport(0, 0, backend.getSize());
    }

private:
    GLFWGLBackend& backend;
};

GLFWGLBackend::GLFWGLBackend(GLFWwindow* window_)
    : mbgl::gl::RendererBackend(mbgl::gfx::ContextMode::Unique),
      mbgl::gfx::Renderable(
          [window_] {
              int fbWidth, fbHeight;
              glfwGetFramebufferSize(window_, &fbWidth, &fbHeight);
              return mbgl::Size{ static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight) };
          }(),
          std::make_unique<GLFWGLRenderableResource>(*this)),
      window(window_) {
}

GLFWGLBackend::~GLFWGLBackend() = default;

void GLFWGLBackend::activate() {
    glfwMakeContextCurrent(window);
}

void GLFWGLBackend::deactivate() {
    glfwMakeContextCurrent(nullptr);
}

mbgl::gl::ProcAddress GLFWGLBackend::getExtensionFunctionPointer(const char* name) {
    return glfwGetProcAddress(name);
}

void GLFWGLBackend::updateAssumedState() {
    assumeFramebufferBinding(0);
    assumeViewport(0, 0, size);
}

mbgl::Size GLFWGLBackend::getSize() const {
    return size;
}

void GLFWGLBackend::setSize(const mbgl::Size newSize) {
    size = newSize;
}
