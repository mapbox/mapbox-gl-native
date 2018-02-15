#include "qmapboxgl_map_renderer.hpp"

#include <QtGlobal>

QMapboxGLMapRenderer::QMapboxGLMapRenderer(qreal pixelRatio,
        mbgl::DefaultFileSource &fs, mbgl::ThreadPool &tp, QMapboxGLSettings::GLContextMode mode)
    : m_renderer(std::make_unique<mbgl::Renderer>(m_backend, pixelRatio, fs, tp, static_cast<mbgl::GLContextMode>(mode)))
    , m_threadWithScheduler(Scheduler::GetCurrent() != nullptr)
{
}

QMapboxGLMapRenderer::~QMapboxGLMapRenderer()
{
    MBGL_VERIFY_THREAD(tid);
}

void QMapboxGLMapRenderer::schedule(std::weak_ptr<mbgl::Mailbox> mailbox)
{
    std::lock_guard<std::mutex> lock(m_taskQueueMutex);
    m_taskQueue.push(mailbox);

    // Need to force the main thread to wake
    // up this thread and process the events.
    emit needsRendering();
}

void QMapboxGLMapRenderer::updateParameters(std::shared_ptr<mbgl::UpdateParameters> newParameters)
{
    std::lock_guard<std::mutex> lock(m_updateMutex);
    m_updateParameters = std::move(newParameters);
}

void QMapboxGLMapRenderer::updateFramebuffer(quint32 fbo, const mbgl::Size &size)
{
    MBGL_VERIFY_THREAD(tid);

    m_backend.updateFramebuffer(fbo, size);
}

void QMapboxGLMapRenderer::render()
{
    MBGL_VERIFY_THREAD(tid);

    std::shared_ptr<mbgl::UpdateParameters> params;
    {
        // Lock on the parameters
        std::lock_guard<std::mutex> lock(m_updateMutex);

        if (!m_updateParameters) {
            return;
        }

        // Hold on to the update parameters during render
        params = m_updateParameters;
    }

    // The OpenGL implementation automatically enables the OpenGL context for us.
    mbgl::BackendScope scope(m_backend, mbgl::BackendScope::ScopeType::Implicit);

    // If we don't have a Scheduler on this thread, which
    // is usually the case for render threads, use this
    // object as scheduler.
    if (!m_threadWithScheduler) {
        Scheduler::SetCurrent(this);
    }

    m_renderer->render(*params);

    if (!m_threadWithScheduler) {
        std::queue<std::weak_ptr<mbgl::Mailbox>> taskQueue;
        {
            std::unique_lock<std::mutex> lock(m_taskQueueMutex);
            std::swap(taskQueue, m_taskQueue);
        }

        while (!taskQueue.empty()) {
            mbgl::Mailbox::maybeReceive(taskQueue.front());
            taskQueue.pop();
        }
    }
}

void QMapboxGLMapRenderer::setObserver(std::shared_ptr<mbgl::RendererObserver> observer)
{
    m_renderer->setObserver(observer.get());
}
