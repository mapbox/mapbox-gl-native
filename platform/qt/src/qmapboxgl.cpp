#include "qmapboxgl_p.hpp"

#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/vec.hpp>

#include <QCoreApplication>
#include <QImage>
#include <QMapboxGL>
#include <QString>
#include <QStringList>

#include <memory>

QMapboxGLSettings::QMapboxGLSettings()
    : m_mapMode(QMapboxGLSettings::ContinuousMap)
    , m_contextMode(QMapboxGLSettings::UniqueGLContext)
    , m_constrainMode(QMapboxGLSettings::ConstrainHeightOnly)
    , m_cacheMaximumSize(mbgl::util::DEFAULT_MAX_CACHE_SIZE)
    , m_cacheDatabasePath(":memory:")
    , m_assetPath(QCoreApplication::applicationDirPath())
{
}

QMapboxGLSettings::MapMode QMapboxGLSettings::mapMode() const
{
    return m_mapMode;
}

void QMapboxGLSettings::setMapMode(MapMode mode)
{
    m_mapMode = mode;
}

QMapboxGLSettings::GLContextMode QMapboxGLSettings::contextMode() const
{
    return m_contextMode;
}

void QMapboxGLSettings::setContextMode(GLContextMode mode)
{
    m_contextMode = mode;
}

QMapboxGLSettings::ConstrainMode QMapboxGLSettings::constrainMode() const
{
    return m_constrainMode;
}

void QMapboxGLSettings::setConstrainMode(ConstrainMode mode)
{
    m_constrainMode = mode;
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
    mbgl::MapMode mapMode = static_cast<mbgl::MapMode>(settings.mapMode());
    mbgl::GLContextMode contextMode = static_cast<mbgl::GLContextMode>(settings.contextMode());
    mbgl::ConstrainMode constrainMode = static_cast<mbgl::ConstrainMode>(settings.constrainMode());

    d_ptr->fileSourceObj->setAccessToken(settings.accessToken().toStdString());
    d_ptr->mapObj = std::make_unique<mbgl::Map>(*d_ptr, *d_ptr->fileSourceObj, mapMode, contextMode, constrainMode);
}

QMapboxGL::~QMapboxGL()
{
    delete d_ptr;
}

void QMapboxGL::cycleDebugOptions()
{
    d_ptr->mapObj->cycleDebugOptions();
}

QString QMapboxGL::styleJSON() const
{
    return QString::fromStdString(d_ptr->mapObj->getStyleJSON());
}

QString QMapboxGL::styleURL() const
{
    return QString::fromStdString(d_ptr->mapObj->getStyleURL());
}

void QMapboxGL::setStyleJSON(const QString &style)
{
    d_ptr->mapObj->setStyleJSON(style.toStdString());
}

void QMapboxGL::setStyleURL(const QString &url)
{
    d_ptr->mapObj->setStyleURL(url.toStdString());
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

double QMapboxGL::scale() const
{
    return d_ptr->mapObj->getScale();
}

void QMapboxGL::setScale(double scale_, const QPointF &center, int milliseconds)
{
    d_ptr->mapObj->setScale(scale_, mbgl::ScreenCoordinate{ center.x(), center.y() }, std::chrono::milliseconds(milliseconds));
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
            mbgl::Duration(std::chrono::milliseconds(milliseconds)));
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
    d_ptr->mapObj->setBearing(degrees, mbgl::Duration(std::chrono::milliseconds(milliseconds)));
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

QMapboxGL::NorthOrientation QMapboxGL::northOrientation() const
{
    using NO = mbgl::NorthOrientation;
    switch (d_ptr->mapObj->getNorthOrientation()) {
        case NO::Rightwards: return NorthRightwards;
        case NO::Downwards: return NorthDownwards;
        case NO::Leftwards: return NorthLeftwards;
        default: return NorthUpwards;
    }
}

void QMapboxGL::setNorthOrientation(NorthOrientation orientation)
{
    using NO = mbgl::NorthOrientation;
    switch (orientation) {
        case NorthRightwards: d_ptr->mapObj->setNorthOrientation(NO::Rightwards); break;
        case NorthDownwards: d_ptr->mapObj->setNorthOrientation(NO::Downwards); break;
        case NorthLeftwards: d_ptr->mapObj->setNorthOrientation(NO::Leftwards); break;
        default: d_ptr->mapObj->setNorthOrientation(NO::Upwards); break;
    }
}

void QMapboxGL::setGestureInProgress(bool inProgress)
{
    d_ptr->mapObj->setGestureInProgress(inProgress);
}

void QMapboxGL::addClass(const QString &className)
{
    d_ptr->mapObj->addClass(className.toStdString());
}

void QMapboxGL::removeClass(const QString &className)
{
    d_ptr->mapObj->removeClass(className.toStdString());
}

bool QMapboxGL::hasClass(const QString &className) const
{
    return d_ptr->mapObj->hasClass(className.toStdString());
}

void QMapboxGL::setClasses(const QStringList &classNames)
{
    std::vector<std::string> mbglClassNames;
    mbglClassNames.reserve(classNames.size());

    for (const QString &className : classNames) {
        mbglClassNames.emplace_back(className.toStdString());
    }

    d_ptr->mapObj->setClasses(mbglClassNames);
}

QStringList QMapboxGL::getClasses() const
{
    QStringList classNames;
    for (const std::string &mbglClass : d_ptr->mapObj->getClasses()) {
        classNames << QString::fromStdString(mbglClass);
    }
    return classNames;
}

mbgl::PointAnnotation fromQMapboxGLPointAnnotation(const QMapboxGL::PointAnnotation &pointAnnotation) {
    const QMapboxGL::Coordinate &coordinate = pointAnnotation.first;
    const QString &icon = pointAnnotation.second;
    return { { coordinate.first, coordinate.second }, icon.toStdString() };
}

QMapboxGL::AnnotationID QMapboxGL::addPointAnnotation(const PointAnnotation &pointAnnotation)
{
    return d_ptr->mapObj->addPointAnnotation(fromQMapboxGLPointAnnotation(pointAnnotation));
}

QMapboxGL::AnnotationIDs QMapboxGL::addPointAnnotations(const PointAnnotations &pointAnnotations)
{
    std::vector<mbgl::PointAnnotation> mbglPointAnnotations;
    mbglPointAnnotations.reserve(pointAnnotations.size());

    for (const PointAnnotation &pointAnnotation : pointAnnotations) {
        mbglPointAnnotations.emplace_back(fromQMapboxGLPointAnnotation(pointAnnotation));
    }

    AnnotationIDs ids;
    for (const mbgl::AnnotationID &id : d_ptr->mapObj->addPointAnnotations(mbglPointAnnotations)) {
        ids << id;
    }

    return ids;
}

mbgl::ShapeAnnotation fromQMapboxGLShapeAnnotation(const QMapboxGL::ShapeAnnotation &shapeAnnotation) {
    const QMapboxGL::CoordinateSegments &segments = shapeAnnotation.first;
    const QString &styleLayer = shapeAnnotation.second;

    mbgl::AnnotationSegments mbglAnnotationSegments;
    mbglAnnotationSegments.reserve(segments.size());

    for (const QMapboxGL::Coordinates &coordinates : segments) {
        mbgl::AnnotationSegment mbglAnnotationSegment;
        mbglAnnotationSegment.reserve(coordinates.size());

        for (const QMapboxGL::Coordinate &coordinate : coordinates) {
            mbgl::LatLng mbglCoordinate(coordinate.first, coordinate.second);
            mbglAnnotationSegment.emplace_back(mbglCoordinate);
        }

        mbglAnnotationSegments.emplace_back(mbglAnnotationSegment);
    }

    return { mbglAnnotationSegments, styleLayer.toStdString() };
}

QMapboxGL::AnnotationID QMapboxGL::addShapeAnnotation(const ShapeAnnotation &shapeAnnotation)
{
    return d_ptr->mapObj->addShapeAnnotation(fromQMapboxGLShapeAnnotation(shapeAnnotation));
}

QMapboxGL::AnnotationIDs QMapboxGL::addShapeAnnotations(const ShapeAnnotations &shapeAnnotations)
{
    std::vector<mbgl::ShapeAnnotation> mbglShapeAnnotations;
    mbglShapeAnnotations.reserve(shapeAnnotations.size());

    for (const ShapeAnnotation &shapeAnnotation : shapeAnnotations) {
        mbglShapeAnnotations.emplace_back(fromQMapboxGLShapeAnnotation(shapeAnnotation));
    }

    AnnotationIDs ids;
    for (const mbgl::AnnotationID &id : d_ptr->mapObj->addShapeAnnotations(mbglShapeAnnotations)) {
        ids << id;
    }

    return ids;
}

void QMapboxGL::removeAnnotation(AnnotationID annotationID)
{
    d_ptr->mapObj->removeAnnotation(annotationID);
}

void QMapboxGL::removeAnnotations(const AnnotationIDs &annotationIDs)
{
    std::vector<mbgl::AnnotationID> mbglAnnotationIds;
    mbglAnnotationIds.reserve(annotationIDs.size());

    for (const AnnotationID annotationID : annotationIDs) {
        mbglAnnotationIds.emplace_back(annotationID);
    }

    d_ptr->mapObj->removeAnnotations(mbglAnnotationIds);
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

void QMapboxGL::connectionEstablished()
{
    d_ptr->connectionEstablished();
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

void QMapboxGLPrivate::connectionEstablished()
{
    mbgl::NetworkStatus::Reachable();
}
