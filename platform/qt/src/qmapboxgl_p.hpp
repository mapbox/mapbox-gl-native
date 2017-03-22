#pragma once

#include "qmapboxgl.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/map/backend.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/geo.hpp>

#include <QObject>
#include <QSize>

class QMapboxGLPrivate : public QObject, public mbgl::View, public mbgl::Backend
{
    Q_OBJECT

public:
    explicit QMapboxGLPrivate(QMapboxGL *, const QMapboxGLSettings &, const QSize &size, qreal pixelRatio);
    virtual ~QMapboxGLPrivate();

    mbgl::Size framebufferSize() const;

    // mbgl::View implementation.
    void bind() final;

    // mbgl::Backend implementation.
    void updateAssumedState() final;
    void invalidate() final;
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

    std::unique_ptr<mbgl::DefaultFileSource> fileSourceObj;
    std::shared_ptr<mbgl::ThreadPool> threadPool;
    std::unique_ptr<mbgl::Map> mapObj;

    bool dirty { false };

private:
    mbgl::gl::ProcAddress initializeExtension(const char*) override;

public slots:
    void connectionEstablished();

signals:
    void needsRendering();
    void mapChanged(QMapboxGL::MapChange);
    void copyrightsChanged(const QString &copyrightsHtml);
};
