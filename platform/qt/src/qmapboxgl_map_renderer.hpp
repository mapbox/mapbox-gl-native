#pragma once

#include "qmapboxgl.hpp"
#include "qmapboxgl_renderer_backend.hpp"

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/shared_thread_pool.hpp>

#include <memory>
#include <mutex>

namespace mbgl {
class Renderer;
class UpdateParameters;
} // namespace mbgl

class QMapboxGLRendererBackend;

class QMapboxGLMapRenderer
{
public:
    QMapboxGLMapRenderer(qreal pixelRatio, mbgl::DefaultFileSource &,
            mbgl::ThreadPool &, QMapboxGLSettings::GLContextMode);
    virtual ~QMapboxGLMapRenderer();

    void render();

    // Thread-safe, called by the Frontend
    void updateParameters(std::shared_ptr<mbgl::UpdateParameters>);
    void updateFramebufferSize(const mbgl::Size &size);

private:
    Q_DISABLE_COPY(QMapboxGLMapRenderer)

    std::mutex m_updateMutex;
    std::shared_ptr<mbgl::UpdateParameters> m_updateParameters;

    QMapboxGLRendererBackend m_backend;
    std::unique_ptr<mbgl::Renderer> m_renderer;
};
