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

    // mbgl::View implementation.
    float getPixelRatio() const;
    void bind() final;
    std::array<uint16_t, 2> getSize() const;
    std::array<uint16_t, 2> getFramebufferSize() const;

    void activate() final {}
    void deactivate() final {}
    void invalidate() final;

    // mbgl::Backend (mbgl::MapObserver) implementation.
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
    void onSourceDidChange() final;

    mbgl::EdgeInsets margins;
    QSize size { 0, 0 };
    QSize fbSize { 0, 0 };

    QMapboxGL *q_ptr { nullptr };

    std::unique_ptr<mbgl::DefaultFileSource> fileSourceObj;
    std::shared_ptr<mbgl::ThreadPool> threadPool;
    std::unique_ptr<mbgl::Map> mapObj;

    bool dirty { false };

public slots:
    void connectionEstablished();

signals:
    void needsRendering();
    void mapChanged(QMapboxGL::MapChange);
    void copyrightsChanged(const QString &copyrightsHtml);
};
