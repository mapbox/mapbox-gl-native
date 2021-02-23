#include "qmapboxgl_map_observer.hpp"

#include "qmapboxgl_p.hpp"

#include <mbgl/util/exception.hpp>

#include <exception>

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

void QMapboxGLMapObserver::onDidFailLoadingMap(mbgl::MapLoadError error, const std::string& what)
{
    emit mapChanged(QMapboxGL::MapChangeDidFailLoadingMap);

    QMapboxGL::MapLoadingFailure type;
    QString description(what.c_str());

    switch (error) {
        case mbgl::MapLoadError::StyleParseError:
            type = QMapboxGL::MapLoadingFailure::StyleParseFailure;
            break;
        case mbgl::MapLoadError::StyleLoadError:
            type = QMapboxGL::MapLoadingFailure::StyleLoadFailure;
            break;
        case mbgl::MapLoadError::NotFoundError:
            type = QMapboxGL::MapLoadingFailure::NotFoundFailure;
            break;
        default:
            type = QMapboxGL::MapLoadingFailure::UnknownFailure;
    }

    emit mapLoadingFailed(type, description);
}

void QMapboxGLMapObserver::onWillStartRenderingFrame()
{
    emit mapChanged(QMapboxGL::MapChangeWillStartRenderingFrame);
}

void QMapboxGLMapObserver::onDidFinishRenderingFrame(mbgl::MapObserver::RenderFrameStatus status)
{
    if (status.mode == mbgl::MapObserver::RenderMode::Partial) {
        emit mapChanged(QMapboxGL::MapChangeDidFinishRenderingFrame);
    } else {
        emit mapChanged(QMapboxGL::MapChangeDidFinishRenderingFrameFullyRendered);
    }
}

void QMapboxGLMapObserver::onWillStartRenderingMap()
{
    emit mapChanged(QMapboxGL::MapChangeWillStartRenderingMap);
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
