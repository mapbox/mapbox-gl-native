#include "qmapboxgl_map_observer.hpp"

#include "qmapboxgl_p.hpp"

QMapboxGLMapObserver::QMapboxGLMapObserver(QMapboxGLPrivate *d)
    : d_ptr(d)
{
}

QMapboxGLMapObserver::~QMapboxGLMapObserver()
{
}

void QMapboxGLMapObserver::onCameraWillChange(mbgl::MapObserver::CameraChangeMode mode)
{
    if (mode == mbgl::MapObserver::CameraChangeMode::Immediate) {
        emit mapChanged(QMapboxGL::MapChangeRegionWillChange);
    } else {
        emit mapChanged(QMapboxGL::MapChangeRegionWillChangeAnimated);
    }
}

void QMapboxGLMapObserver::onCameraIsChanging()
{
    emit mapChanged(QMapboxGL::MapChangeRegionIsChanging);
}

void QMapboxGLMapObserver::onCameraDidChange(mbgl::MapObserver::CameraChangeMode mode)
{
    if (mode == mbgl::MapObserver::CameraChangeMode::Immediate) {
        emit mapChanged(QMapboxGL::MapChangeRegionDidChange);
    } else {
        emit mapChanged(QMapboxGL::MapChangeRegionDidChangeAnimated);
    }
}

void QMapboxGLMapObserver::onWillStartLoadingMap()
{
    emit mapChanged(QMapboxGL::MapChangeWillStartLoadingMap);
}

void QMapboxGLMapObserver::onDidFinishLoadingMap()
{
    emit mapChanged(QMapboxGL::MapChangeDidFinishLoadingMap);
}

void QMapboxGLMapObserver::onDidFailLoadingMap(std::exception_ptr)
{
    emit mapChanged(QMapboxGL::MapChangeDidFailLoadingMap);
}

void QMapboxGLMapObserver::onWillStartRenderingFrame()
{
    emit mapChanged(QMapboxGL::MapChangeWillStartRenderingFrame);
}

void QMapboxGLMapObserver::onDidFinishRenderingFrame(mbgl::MapObserver::RenderMode mode)
{
    if (mode == mbgl::MapObserver::RenderMode::Partial) {
        emit mapChanged(QMapboxGL::MapChangeDidFinishRenderingFrame);
    } else {
        emit mapChanged(QMapboxGL::MapChangeDidFinishRenderingFrameFullyRendered);
    }
}

void QMapboxGLMapObserver::onWillStartRenderingMap()
{
    emit mapChanged(QMapboxGL::MapChangeWillStartLoadingMap);
}

void QMapboxGLMapObserver::onDidFinishRenderingMap(mbgl::MapObserver::RenderMode mode)
{
    if (mode == mbgl::MapObserver::RenderMode::Partial) {
        emit mapChanged(QMapboxGL::MapChangeDidFinishRenderingMap);
    } else {
        emit mapChanged(QMapboxGL::MapChangeDidFinishRenderingMapFullyRendered);
    }
}

void QMapboxGLMapObserver::onDidFinishLoadingStyle()
{
    emit mapChanged(QMapboxGL::MapChangeDidFinishLoadingStyle);
}

void QMapboxGLMapObserver::onSourceChanged(mbgl::style::Source&)
{
    std::string attribution;
    for (const auto& source : d_ptr->mapObj->getStyle().getSources()) {
        // Avoid duplicates by using the most complete attribution HTML snippet.
        if (source->getAttribution() && (attribution.size() < source->getAttribution()->size()))
            attribution = *source->getAttribution();
    }
    emit copyrightsChanged(QString::fromStdString(attribution));
    emit mapChanged(QMapboxGL::MapChangeSourceDidChange);
}
