#pragma once

#include "qmapboxgl.hpp"

#include <mbgl/map/map_observer.hpp>
#include <mbgl/style/style.hpp>

#include <QObject>

#include <exception>
#include <memory>

class QMapboxGLPrivate;

class QMapboxGLMapObserver : public QObject, public mbgl::MapObserver
{
    Q_OBJECT

public:
    explicit QMapboxGLMapObserver(QMapboxGLPrivate *);
    virtual ~QMapboxGLMapObserver();

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

signals:
    void mapChanged(QMapboxGL::MapChange);
    void mapLoadingFailed(QMapboxGL::MapLoadingFailure, const QString &reason);
    void copyrightsChanged(const QString &copyrightsHtml);

private:
    Q_DISABLE_COPY(QMapboxGLMapObserver)

    QMapboxGLPrivate *d_ptr;
};
