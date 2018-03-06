#pragma once

#include "qmapboxgl.hpp"
#include "qmapboxgl_map_observer.hpp"
#include "qmapboxgl_map_renderer.hpp"

#include <mbgl/actor/actor.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/resource_transform.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/geo.hpp>

#include <QObject>
#include <QSize>

#include <atomic>
#include <memory>

class QMapboxGLPrivate : public QObject, public mbgl::RendererFrontend
{
    Q_OBJECT

public:
    explicit QMapboxGLPrivate(QMapboxGL *, const QMapboxGLSettings &, const QSize &size, qreal pixelRatio);
    virtual ~QMapboxGLPrivate();

    // mbgl::RendererFrontend implementation.
    void reset() final {}
    void setObserver(mbgl::RendererObserver &) final;
    void update(std::shared_ptr<mbgl::UpdateParameters>) final;

    // These need to be called on the same thread.
    void createRenderer();
    void destroyRenderer();
    void render();
    void setFramebufferObject(quint32 fbo, const QSize& size);

    mbgl::EdgeInsets margins;
    std::unique_ptr<mbgl::Map> mapObj;

public slots:
    void requestRendering();

signals:
    void needsRendering();

private:
    Q_DISABLE_COPY(QMapboxGLPrivate)

    std::recursive_mutex m_mapRendererMutex;
    std::shared_ptr<mbgl::RendererObserver> m_rendererObserver;

    std::unique_ptr<QMapboxGLMapObserver> m_mapObserver;
    std::shared_ptr<mbgl::DefaultFileSource> m_fileSourceObj;
    std::shared_ptr<mbgl::ThreadPool> m_threadPool;
    std::unique_ptr<QMapboxGLMapRenderer> m_mapRenderer;
    std::unique_ptr<mbgl::Actor<mbgl::ResourceTransform>> m_resourceTransform;

    QMapboxGLSettings::GLContextMode m_mode;
    qreal m_pixelRatio;

    std::atomic_flag m_renderQueued = ATOMIC_FLAG_INIT;
};
