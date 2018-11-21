#pragma once

#include "qmapboxgl.hpp"
#include "qmapboxgl_renderer_backend.hpp"

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/shared_thread_pool.hpp>
#include <mbgl/util/util.hpp>

#include <QtGlobal>

#include <memory>
#include <mutex>

namespace mbgl {
class Renderer;
class UpdateParameters;
} // namespace mbgl

class QMapboxGLRendererBackend;

class QMapboxGLMapRenderer : public QObject
{
    Q_OBJECT

public:
    QMapboxGLMapRenderer(qreal pixelRatio, mbgl::DefaultFileSource &,
            mbgl::ThreadPool &, QMapboxGLSettings::GLContextMode);
    virtual ~QMapboxGLMapRenderer();

    void render();
    void updateFramebuffer(quint32 fbo, const mbgl::Size &size);
    void setObserver(std::shared_ptr<mbgl::RendererObserver>);

    // Thread-safe, called by the Frontend
    void updateParameters(std::shared_ptr<mbgl::UpdateParameters>);

signals:
    void needsRendering();

private:
    MBGL_STORE_THREAD(tid)

    Q_DISABLE_COPY(QMapboxGLMapRenderer)

    std::mutex m_updateMutex;
    std::shared_ptr<mbgl::UpdateParameters> m_updateParameters;

    QMapboxGLRendererBackend m_backend;
    std::unique_ptr<mbgl::Renderer> m_renderer;

    bool m_forceScheduler;
};
