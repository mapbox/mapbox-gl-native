#pragma once

#include "qmapboxgl.hpp"

#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/shared_thread_pool.hpp>

class QMapboxGLRendererBackend : public mbgl::RendererBackend
{
public:
    QMapboxGLRendererBackend() = default;
    virtual ~QMapboxGLRendererBackend() = default;

    // mbgl::RendererBackend implementation
    void updateAssumedState() final;
    void bind() final;
    mbgl::Size getFramebufferSize() const final;

    void updateFramebuffer(quint32 fbo, const mbgl::Size &);

protected:
    mbgl::gl::ProcAddress getExtensionFunctionPointer(const char*) final;

    // No-op, implicit mode.
    void activate() final {}
    void deactivate() final {}

private:
    quint32 m_fbo = 0;
    mbgl::Size m_size = { 0, 0 };

    Q_DISABLE_COPY(QMapboxGLRendererBackend)
};
