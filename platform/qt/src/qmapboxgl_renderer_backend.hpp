#pragma once

#include "qmapboxgl.hpp"

#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gl/renderer_backend.hpp>

class QMapboxGLRendererBackend final : public mbgl::gl::RendererBackend,
                                       public mbgl::gfx::Renderable {
public:
    QMapboxGLRendererBackend(mbgl::gfx::ContextMode);
    ~QMapboxGLRendererBackend() override;

    void updateFramebuffer(quint32 fbo, const mbgl::Size&);
    void restoreFramebufferBinding();

    // mbgl::gfx::RendererBackend implementation
public:
    mbgl::gfx::Renderable& getDefaultRenderable() override {
        return *this;
    }

protected:
    // No-op, implicit mode.
    void activate() override {}
    void deactivate() override {}

    // mbgl::gl::RendererBackend implementation
protected:
    mbgl::gl::ProcAddress getExtensionFunctionPointer(const char*) override;
    void updateAssumedState() override;

private:
    quint32 m_fbo = 0;

    Q_DISABLE_COPY(QMapboxGLRendererBackend)
};
