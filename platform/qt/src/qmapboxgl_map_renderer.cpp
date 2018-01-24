#include "qmapboxgl_map_renderer.hpp"

#include <QtGlobal>

QMapboxGLMapRenderer::QMapboxGLMapRenderer(qreal pixelRatio,
        mbgl::DefaultFileSource &fs, mbgl::ThreadPool &tp, QMapboxGLSettings::GLContextMode mode)
    : m_renderer(std::make_unique<mbgl::Renderer>(m_backend, pixelRatio, fs, tp, static_cast<mbgl::GLContextMode>(mode)))
{
}

QMapboxGLMapRenderer::~QMapboxGLMapRenderer()
{
}

void QMapboxGLMapRenderer::updateParameters(std::shared_ptr<mbgl::UpdateParameters> newParameters)
{
    std::lock_guard<std::mutex> lock(m_updateMutex);
    m_updateParameters = std::move(newParameters);
}

void QMapboxGLMapRenderer::updateFramebufferSize(const mbgl::Size &size)
{
    std::lock_guard<std::mutex> lock(m_updateMutex);
    m_backend.setFramebufferSize(size);
}

void QMapboxGLMapRenderer::render()
{
    std::shared_ptr<mbgl::UpdateParameters> params;
    {
        // Lock on the parameters
        std::unique_lock<std::mutex> lock(m_updateMutex);
        if (!m_updateParameters) return;

        // Hold on to the update parameters during render
        params = m_updateParameters;
    }

    // The OpenGL implementation automatically enables the OpenGL context for us.
    mbgl::BackendScope scope(m_backend, mbgl::BackendScope::ScopeType::Implicit);

    m_renderer->render(*params);
}

void QMapboxGLMapRenderer::setObserver(std::shared_ptr<mbgl::RendererObserver> observer)
{
    m_renderer->setObserver(observer.get());
}
