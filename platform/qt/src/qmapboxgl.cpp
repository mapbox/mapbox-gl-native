#include "qmapboxgl_p.hpp"

#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/vec.hpp>

#include <QCoreApplication>
#include <QImage>
#include <QMapboxGL>
#include <QString>

#include <memory>

QMapboxGLSettings::QMapboxGLSettings()
    : m_cacheMaximumSize(mbgl::util::DEFAULT_MAX_CACHE_SIZE)
    , m_cacheDatabasePath(":memory:")
    , m_assetPath(QCoreApplication::applicationDirPath())
{
}

unsigned QMapboxGLSettings::cacheDatabaseMaximumSize() const
{
    return m_cacheMaximumSize;
}

void QMapboxGLSettings::setCacheDatabaseMaximumSize(unsigned size)
{
    m_cacheMaximumSize = size;
}

QString QMapboxGLSettings::cacheDatabasePath() const
{
    return m_cacheDatabasePath;
}

void QMapboxGLSettings::setCacheDatabasePath(const QString &path)
{
    m_cacheDatabasePath = path;
}

QString QMapboxGLSettings::assetPath() const
{
    return m_assetPath;
}

void QMapboxGLSettings::setAssetPath(const QString &path)
{
    m_assetPath = path;
}

QString QMapboxGLSettings::accessToken() const {
    return m_accessToken;
}

void QMapboxGLSettings::setAccessToken(const QString &token)
{
    m_accessToken = token;
}

QMapboxGL::QMapboxGL(QObject *parent_, const QMapboxGLSettings &settings)
    : QObject(parent_)
    , d_ptr(new QMapboxGLPrivate(this, settings))
{
    d_ptr->fileSourceObj->setAccessToken(settings.accessToken().toStdString());

    d_ptr->mapObj = std::make_unique<mbgl::Map>(*d_ptr, *d_ptr->fileSourceObj,
            mbgl::MapMode::Continuous, mbgl::GLContextMode::Shared);
}

QMapboxGL::~QMapboxGL()
{
    delete d_ptr;
}

void QMapboxGL::setStyleJSON(const QString &style)
{
    d_ptr->mapObj->setStyleJSON(style.toUtf8().constData());
}

void QMapboxGL::setStyleURL(const QString &url)
{
    d_ptr->mapObj->setStyleURL(url.toUtf8().constData());
}

double QMapboxGL::latitude() const
{
    return d_ptr->mapObj->getLatLng().latitude;
}

void QMapboxGL::setLatitude(double latitude_)
{
    d_ptr->mapObj->setLatLng(mbgl::LatLng { latitude_, longitude() });
}

double QMapboxGL::longitude() const
{
    return d_ptr->mapObj->getLatLng().longitude;
}

void QMapboxGL::setLongitude(double longitude_)
{
    d_ptr->mapObj->setLatLng(mbgl::LatLng { latitude(), longitude_ });
}

double QMapboxGL::zoom() const
{
    return d_ptr->mapObj->getZoom();
}

void QMapboxGL::setZoom(double zoom_, int milliseconds)
{
    d_ptr->mapObj->setZoom(zoom_, std::chrono::milliseconds(milliseconds));
}

double QMapboxGL::minimumZoom() const
{
    return d_ptr->mapObj->getMinZoom();
}

double QMapboxGL::maximumZoom() const
{
    return d_ptr->mapObj->getMaxZoom();
}

QMapboxGL::Coordinate QMapboxGL::coordinate() const
{
    const mbgl::LatLng& latLng = d_ptr->mapObj->getLatLng();
    return Coordinate(latLng.latitude, latLng.longitude);
}

void QMapboxGL::setCoordinate(const Coordinate &coordinate_, int milliseconds)
{
    d_ptr->mapObj->setLatLng(
            mbgl::LatLng { coordinate_.first, coordinate_.second },
            std::chrono::milliseconds(milliseconds));
}

void QMapboxGL::setCoordinateZoom(const Coordinate &coordinate_, double zoom_, int milliseconds)
{
    d_ptr->mapObj->setLatLngZoom(
            mbgl::LatLng { coordinate_.first, coordinate_.second }, zoom_,
            std::chrono::milliseconds(milliseconds));
}

double QMapboxGL::bearing() const
{
    return d_ptr->mapObj->getBearing();
}

void QMapboxGL::setBearing(double degrees, int milliseconds)
{
    d_ptr->mapObj->setBearing(degrees, std::chrono::milliseconds(milliseconds));
}

void QMapboxGL::setBearing(double degrees, const QPointF &center)
{
    d_ptr->mapObj->setBearing(degrees, mbgl::ScreenCoordinate { center.x(), center.y() });
}

double QMapboxGL::pitch() const
{
    return d_ptr->mapObj->getPitch();
}

void QMapboxGL::setPitch(double pitch_, int milliseconds)
{
    d_ptr->mapObj->setPitch(pitch_, std::chrono::milliseconds(milliseconds));
}

void QMapboxGL::setGestureInProgress(bool inProgress)
{
    d_ptr->mapObj->setGestureInProgress(inProgress);
}

bool QMapboxGL::isRotating() const
{
    return d_ptr->mapObj->isRotating();
}

bool QMapboxGL::isScaling() const
{
    return d_ptr->mapObj->isScaling();
}

bool QMapboxGL::isPanning() const
{
    return d_ptr->mapObj->isPanning();
}

bool QMapboxGL::isFullyLoaded() const
{
    return d_ptr->mapObj->isFullyLoaded();
}

void QMapboxGL::moveBy(const QPointF &offset)
{
    d_ptr->mapObj->moveBy(mbgl::ScreenCoordinate { offset.x(), offset.y() });
}

void QMapboxGL::scaleBy(double scale_, const QPointF &center, int milliseconds) {
    d_ptr->mapObj->scaleBy(
            scale_, mbgl::ScreenCoordinate { center.x(), center.y() },
            std::chrono::milliseconds(milliseconds));
}

void QMapboxGL::rotateBy(const QPointF &first, const QPointF &second)
{
    d_ptr->mapObj->rotateBy(
            mbgl::ScreenCoordinate { first.x(), first.y() },
            mbgl::ScreenCoordinate { second.x(), second.y() });
}

void QMapboxGL::resize(const QSize& size)
{
    if (d_ptr->size == size) return;

    d_ptr->size = size;
    d_ptr->mapObj->update(mbgl::Update::Dimensions);
}

void QMapboxGL::addAnnotationIcon(const QString &name, const QImage &sprite)
{
    if (sprite.isNull()) return;

    const QImage swapped = sprite
        .rgbSwapped()
        .convertToFormat(QImage::Format_ARGB32_Premultiplied);

    auto img = std::make_unique<uint8_t[]>(swapped.byteCount());
    memcpy(img.get(), swapped.constBits(), swapped.byteCount());

    d_ptr->mapObj->addAnnotationIcon(name.toStdString(), std::make_shared<mbgl::SpriteImage>(
        mbgl::PremultipliedImage { size_t(swapped.width()), size_t(swapped.height()), std::move(img) }, 1.0));
}

QPointF QMapboxGL::pixelForCoordinate(const Coordinate &coordinate_) const
{
    const mbgl::vec2<double> pixel =
        d_ptr->mapObj->pixelForLatLng(mbgl::LatLng { coordinate_.first, coordinate_.second });

    return QPointF(pixel.x, pixel.y);
}

QMapboxGL::Coordinate QMapboxGL::coordinateForPixel(const QPointF &pixel) const
{
    const mbgl::LatLng latLng =
        d_ptr->mapObj->latLngForPixel(mbgl::ScreenCoordinate { pixel.x(), pixel.y() });

    return Coordinate(latLng.latitude, latLng.longitude);
}

void QMapboxGL::render()
{
    d_ptr->mapObj->render();
}

QMapboxGLPrivate::QMapboxGLPrivate(QMapboxGL *q, const QMapboxGLSettings &settings)
    : QObject(q)
    , size(0, 0)
    , q_ptr(q)
    , fileSourceObj(std::make_unique<mbgl::DefaultFileSource>(
        settings.cacheDatabasePath().toStdString(),
        settings.assetPath().toStdString(),
        settings.cacheDatabaseMaximumSize()))
    , mapObj(std::make_unique<mbgl::Map>(
        *this, *fileSourceObj,
        mbgl::MapMode::Continuous,
        mbgl::GLContextMode::Shared))
{
    connect(this, SIGNAL(needsRendering()), q_ptr, SIGNAL(needsRendering()));
    connect(this, SIGNAL(mapRegionDidChange()), q_ptr, SIGNAL(mapRegionDidChange()));
}

QMapboxGLPrivate::~QMapboxGLPrivate()
{
}

float QMapboxGLPrivate::getPixelRatio() const
{
    // FIXME: Should handle pixel ratio.
    return 1.0;
}

std::array<uint16_t, 2> QMapboxGLPrivate::getSize() const
{
    return {{ static_cast<uint16_t>(size.width()), static_cast<uint16_t>(size.height()) }};
}

std::array<uint16_t, 2> QMapboxGLPrivate::getFramebufferSize() const
{
    return getSize();
}

void QMapboxGLPrivate::invalidate()
{
    emit needsRendering();
}

void QMapboxGLPrivate::notifyMapChange(mbgl::MapChange change)
{
    // Map thread.
    switch (change) {
    case mbgl::MapChangeRegionDidChange:
    case mbgl::MapChangeRegionDidChangeAnimated:
    case mbgl::MapChangeRegionIsChanging:
        emit mapRegionDidChange();
        break;
    default:
        break;
    }
}
