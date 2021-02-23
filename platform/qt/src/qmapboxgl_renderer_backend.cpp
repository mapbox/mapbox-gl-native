#include "qmapboxgl_renderer_backend.hpp"

#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gl/renderable_resource.hpp>

#include <QOpenGLContext>
#include <QtGlobal>

class QMapboxGLRenderableResource final : public mbgl::gl::RenderableResource {
public:
    QMapboxGLRenderableResource(QMapboxGLRendererBackend& backend_) : backend(backend_) {
    }

    void bind() override {
        assert(mbgl::gfx::BackendScope::exists());
        backend.restoreFramebufferBinding();
        backend.setViewport(0, 0, backend.getSize());
    }

private:
    QMapboxGLRendererBackend& backend;
};

QMapboxGLRendererBackend::QMapboxGLRendererBackend(const mbgl::gfx::ContextMode contextMode_)
    : mbgl::gl::RendererBackend(contextMode_),
      mbgl::gfx::Renderable({ 0, 0 }, std::make_unique<QMapboxGLRenderableResource>(*this)) {
}

QMapboxGLRendererBackend::~QMapboxGLRendererBackend() = default;

void QMapboxGLRendererBackend::updateAssumedState() {
    assumeFramebufferBinding(ImplicitFramebufferBinding);
    assumeViewport(0, 0, size);
}

void QMapboxGLRendererBackend::restoreFramebufferBinding() {
    setFramebufferBinding(m_fbo);
}

void QMapboxGLRendererBackend::updateFramebuffer(quint32 fbo, const mbgl::Size& newSize) {
    m_fbo = fbo;
    size = newSize;
}

/*!
    Initializes an OpenGL extension function such as Vertex Array Objects (VAOs),
    required by Mapbox GL Native engine.
*/
mbgl::gl::ProcAddress QMapboxGLRendererBackend::getExtensionFunctionPointer(const char* name) {
    QOpenGLContext* thisContext = QOpenGLContext::currentContext();
    return thisContext->getProcAddress(name);
}
