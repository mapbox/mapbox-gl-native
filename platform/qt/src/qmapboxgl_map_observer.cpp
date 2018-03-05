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

void QMapboxGLMapObserver::onDidFailLoadingMap(std::exception_ptr exception)
{
    emit mapChanged(QMapboxGL::MapChangeDidFailLoadingMap);

    QMapboxGL::MapLoadingFailure type;
    QString description;

    try {
        std::rethrow_exception(exception);
    } catch (const mbgl::util::StyleParseException& e) {
        type = QMapboxGL::MapLoadingFailure::StyleParseFailure;
        description = e.what();
    } catch (const mbgl::util::StyleLoadException& e) {
        type = QMapboxGL::MapLoadingFailure::StyleLoadFailure;
        description = e.what();
    } catch (const mbgl::util::NotFoundException& e) {
        type = QMapboxGL::MapLoadingFailure::NotFoundFailure;
        description = e.what();
    } catch (const std::exception& e) {
        type = QMapboxGL::MapLoadingFailure::UnknownFailure;
        description = e.what();
    }

    emit mapLoadingFailed(type, description);
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
