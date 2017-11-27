#pragma once

#include "qmapboxgl.hpp"
#include "qmapboxgl_renderer_frontend_p.hpp"

#include <mbgl/actor/actor.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/storage/resource_transform.hpp>

#include <QObject>
#include <QSize>

#include <memory>

class QMapboxGLPrivate : public QObject, public mbgl::RendererBackend, public mbgl::MapObserver
{
    Q_OBJECT

public:
    explicit QMapboxGLPrivate(QMapboxGL *, const QMapboxGLSettings &, const QSize &size, qreal pixelRatio);
    virtual ~QMapboxGLPrivate();


    // mbgl::RendererBackend implementation.
    void bind() final;
    mbgl::Size getFramebufferSize() const final;
    void updateAssumedState() final;
    void activate() final {}
    void deactivate() final {}

    // mbgl::MapObserver implementation.
    void onCameraWillChange(mbgl::MapObserver::CameraChangeMode) final;
    void onCameraIsChanging() final;
    void onCameraDidChange(mbgl::MapObserver::CameraChangeMode) final;
    void onWillStartLoadingMap() final;
    void onDidFinishLoadingMap() final;
    void onDidFailLoadingMap(std::exception_ptr) final;
    void onWillStartRenderingFrame() final;
    void onDidFinishRenderingFrame(mbgl::MapObserver::RenderMode) final;
    void onWillStartRenderingMap() final;
    void onDidFinishRenderingMap(mbgl::MapObserver::RenderMode) final;
    void onDidFinishLoadingStyle() final;
    void onSourceChanged(mbgl::style::Source&) final;

    mbgl::EdgeInsets margins;
    QSize size { 0, 0 };
    QSize fbSize { 0, 0 };
    quint32 fbObject = 0;

    QMapboxGL *q_ptr { nullptr };

    std::shared_ptr<mbgl::DefaultFileSource> fileSourceObj;
    std::shared_ptr<mbgl::ThreadPool> threadPool;
    std::unique_ptr<QMapboxGLRendererFrontend> frontend;
    std::unique_ptr<mbgl::Map> mapObj;

    bool dirty { false };

private:
    mbgl::gl::ProcAddress getExtensionFunctionPointer(const char*) override;

public slots:
    void connectionEstablished();
    void invalidate();
    void render();

signals:
    void needsRendering();
    void mapChanged(QMapboxGL::MapChange);
    void copyrightsChanged(const QString &copyrightsHtml);

private:
    std::unique_ptr< mbgl::Actor<mbgl::ResourceTransform> > m_resourceTransform;
};
