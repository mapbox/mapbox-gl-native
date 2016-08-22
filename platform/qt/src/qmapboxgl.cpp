#include "qmapboxgl_p.hpp"

#include "qt_conversion.hpp"
#include "qt_geojson.hpp"

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/traits.hpp>

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <QWindow>
#else
#include <QCoreApplication>
#endif

#include <QDebug>
#include <QImage>
#include <QMapboxGL>
#include <QMargins>
#include <QString>
#include <QStringList>
#include <QThreadStorage>

#include <memory>

using namespace QMapbox;

// mbgl::MapMode
static_assert(mbgl::underlying_type(QMapboxGLSettings::ContinuousMap) == mbgl::underlying_type(mbgl::MapMode::Continuous), "error");
static_assert(mbgl::underlying_type(QMapboxGLSettings::StillMap) == mbgl::underlying_type(mbgl::MapMode::Still), "error");

// mbgl::GLContextMode
static_assert(mbgl::underlying_type(QMapboxGLSettings::UniqueGLContext) == mbgl::underlying_type(mbgl::GLContextMode::Unique), "error");
static_assert(mbgl::underlying_type(QMapboxGLSettings::SharedGLContext) == mbgl::underlying_type(mbgl::GLContextMode::Shared), "error");

// mbgl::ConstrainMode
static_assert(mbgl::underlying_type(QMapboxGLSettings::NoConstrain) == mbgl::underlying_type(mbgl::ConstrainMode::None), "error");
static_assert(mbgl::underlying_type(QMapboxGLSettings::ConstrainHeightOnly) == mbgl::underlying_type(mbgl::ConstrainMode::HeightOnly), "error");
static_assert(mbgl::underlying_type(QMapboxGLSettings::ConstrainWidthAndHeight) == mbgl::underlying_type(mbgl::ConstrainMode::WidthAndHeight), "error");

// mbgl::ViewportMode
static_assert(mbgl::underlying_type(QMapboxGLSettings::DefaultViewport) == mbgl::underlying_type(mbgl::ViewportMode::Default), "error");
static_assert(mbgl::underlying_type(QMapboxGLSettings::FlippedYViewport) == mbgl::underlying_type(mbgl::ViewportMode::FlippedY), "error");

// mbgl::NorthOrientation
static_assert(mbgl::underlying_type(QMapboxGL::NorthUpwards) == mbgl::underlying_type(mbgl::NorthOrientation::Upwards), "error");
static_assert(mbgl::underlying_type(QMapboxGL::NorthRightwards) == mbgl::underlying_type(mbgl::NorthOrientation::Rightwards), "error");
static_assert(mbgl::underlying_type(QMapboxGL::NorthDownwards) == mbgl::underlying_type(mbgl::NorthOrientation::Downwards), "error");
static_assert(mbgl::underlying_type(QMapboxGL::NorthLeftwards) == mbgl::underlying_type(mbgl::NorthOrientation::Leftwards), "error");

// mbgl::MapChange
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeRegionWillChange) == mbgl::underlying_type(mbgl::MapChangeRegionWillChange), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeRegionWillChangeAnimated) == mbgl::underlying_type(mbgl::MapChangeRegionWillChangeAnimated), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeRegionIsChanging) == mbgl::underlying_type(mbgl::MapChangeRegionIsChanging), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeRegionDidChange) == mbgl::underlying_type(mbgl::MapChangeRegionDidChange), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeRegionDidChangeAnimated) == mbgl::underlying_type(mbgl::MapChangeRegionDidChangeAnimated), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeWillStartLoadingMap) == mbgl::underlying_type(mbgl::MapChangeWillStartLoadingMap), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeDidFinishLoadingMap) == mbgl::underlying_type(mbgl::MapChangeDidFinishLoadingMap), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeDidFailLoadingMap) == mbgl::underlying_type(mbgl::MapChangeDidFailLoadingMap), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeWillStartRenderingFrame) == mbgl::underlying_type(mbgl::MapChangeWillStartRenderingFrame), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeDidFinishRenderingFrame) == mbgl::underlying_type(mbgl::MapChangeDidFinishRenderingFrame), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeDidFinishRenderingFrameFullyRendered) == mbgl::underlying_type(mbgl::MapChangeDidFinishRenderingFrameFullyRendered), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeWillStartRenderingMap) == mbgl::underlying_type(mbgl::MapChangeWillStartRenderingMap), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeDidFinishRenderingMap) == mbgl::underlying_type(mbgl::MapChangeDidFinishRenderingMap), "error");
static_assert(mbgl::underlying_type(QMapboxGL::MapChangeDidFinishRenderingMapFullyRendered) == mbgl::underlying_type(mbgl::MapChangeDidFinishRenderingMapFullyRendered), "error");

namespace {

QThreadStorage<std::shared_ptr<mbgl::util::RunLoop>> loop;

// Convertion helper functions.

auto fromQMapboxGLShapeAnnotation(const ShapeAnnotation &shapeAnnotation) {
    const LineString &lineString = shapeAnnotation.first;
    const QString &styleLayer = shapeAnnotation.second;

    mbgl::LineString<double> mbglLineString;
    mbglLineString.reserve(lineString.size());

    for (const Coordinate &coordinate : lineString) {
        mbglLineString.emplace_back(mbgl::Point<double> { coordinate.first, coordinate.second });
    }

    return mbgl::StyleSourcedAnnotation { std::move(mbglLineString), styleLayer.toStdString() };
}

auto fromQMapboxTransitionOptions(const QMapbox::TransitionOptions &options) {
    auto convert = [](auto& value) -> mbgl::optional<mbgl::Duration> {
        if (value.isValid()) {
            return std::chrono::duration_cast<mbgl::Duration>(mbgl::Milliseconds(value.template value<qint64>()));
        };
        return {};
    };
    return mbgl::style::TransitionOptions { convert(options.duration), convert(options.delay) };
}

auto toQMapboxTransitionOptions(const mbgl::style::TransitionOptions &options) {
    auto convert = [](auto& value) -> QVariant {
        if (value) {
            return qint64(std::chrono::duration_cast<mbgl::Milliseconds>(*value).count());
        }
        return {};
    };
    return QMapbox::TransitionOptions { convert(options.duration), convert(options.delay) };
}

auto fromQStringList(const QStringList &list)
{
    std::vector<std::string> strings;
    strings.reserve(list.size());
    for (const QString &string : list) {
        strings.push_back(string.toStdString());
    }
    return strings;
}

}

/*!
    \class QMapboxGLSettings
    \brief The QMapboxGLSettings class stores the initial configuration for QMapboxGL.

    \inmodule Mapbox Qt SDK

    QMapboxGLSettings is used to configure QMapboxGL at the moment of its creation.
    Once created, the QMapboxGLSettings of a QMapboxGL can no longer be changed.

    \since 4.7
*/

/*!
    \enum QMapboxGLSettings::GLContextMode

    This enum set the expectations towards the GL state.

    \value UniqueGLContext  The GL context is only used by QMapboxGL, so it is not
    reset before each rendering. Use this mode if the intention is to only draw a
    fullscreen map.

    \value SharedGLContext  The GL context is shared and the state will be restored
    before rendering. This mode is safer when GL calls are performed prior of after
    we call QMapboxGL::render for rendering a map.

    \sa contextMode()
*/

/*!
    Constructs a QMapboxGLSettings object with the default values. The default
    configuration is valid for initializing a QMapboxGL.
*/
QMapboxGLSettings::QMapboxGLSettings()
    : m_mapMode(QMapboxGLSettings::ContinuousMap)
    , m_contextMode(QMapboxGLSettings::SharedGLContext)
    , m_constrainMode(QMapboxGLSettings::ConstrainHeightOnly)
    , m_viewportMode(QMapboxGLSettings::DefaultViewport)
    , m_cacheMaximumSize(mbgl::util::DEFAULT_MAX_CACHE_SIZE)
    , m_cacheDatabasePath(":memory:")
    , m_assetPath(QCoreApplication::applicationDirPath())
    , m_accessToken(qgetenv("MAPBOX_ACCESS_TOKEN"))
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

QMapboxGLSettings::ViewportMode QMapboxGLSettings::viewportMode() const
{
    return m_viewportMode;
}

void QMapboxGLSettings::setViewportMode(ViewportMode mode)
{
    m_viewportMode = mode;
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

/*!
    \class QMapboxGL
    \brief The QMapboxGL class is a Qt wrapper for the Mapbox GL Native engine.

    \inmodule Mapbox Qt SDK

    QMapboxGL is a Qt friendly version the Mapbox GL Native engine using Qt types
    and deep integration with Qt event loop. QMapboxGL relies as much as possible
    on Qt, trying to minimize the external dependencies. For instance it will use
    QNetworkAccessManager for HTTP requests and QString for UTF-8 manipulation.

    QMapboxGL is not thread-safe and it is assumed that it will be accessed from
    the same thread as the thread where the GL context lives.

    \since 4.7
*/

/*!
    Constructs a QMapboxGL object with \a settings and sets \a parent as the parent
    object. The \a settings cannot be changed after the object is constructed.
*/
QMapboxGL::QMapboxGL(QObject *parent, const QMapboxGLSettings &settings)
    : QObject(parent)
{
    // Multiple QMapboxGL running on the same thread
    // will share the same mbgl::util::RunLoop
    if (!loop.hasLocalData()) {
        loop.setLocalData(std::make_shared<mbgl::util::RunLoop>());
    }

    d_ptr = new QMapboxGLPrivate(this, settings);
}

QMapboxGL::~QMapboxGL()
{
    delete d_ptr;
}

void QMapboxGL::cycleDebugOptions()
{
    d_ptr->mapObj->cycleDebugOptions();
}

QString QMapboxGL::styleJson() const
{
    return QString::fromStdString(d_ptr->mapObj->getStyleJSON());
}

QString QMapboxGL::styleUrl() const
{
    return QString::fromStdString(d_ptr->mapObj->getStyleURL());
}

/*!
    Sets a new \a style from a JSON that must conform with the
    \l {https://www.mapbox.com/mapbox-gl-style-spec/}
    {Mapbox Style Specification}.

    \note In case of a invalid style it will trigger a mapChanged
    signal with QMapboxGL::MapChangeDidFailLoadingMap as argument.
*/
void QMapboxGL::setStyleJson(const QString &style)
{
    d_ptr->mapObj->setStyleJSON(style.toStdString());
}

/*!
    Sets a URL for fetching a JSON that will be later feed to
    setStyleJson. URLs using the Mapbox scheme (\a mapbox://) are
    also accepted and translated automatically to an actual HTTPS
    request.

    The Mapbox scheme is not enforced by any means and a style can
    be fetched from anything that QNetworkAccessManager can handle.

    \note In case of a invalid style it will trigger a mapChanged
    signal with QMapboxGL::MapChangeDidFailLoadingMap as argument.
*/
void QMapboxGL::setStyleUrl(const QString &url)
{
    d_ptr->mapObj->setStyleURL(url.toStdString());
}

double QMapboxGL::latitude() const
{
    return d_ptr->mapObj->getLatLng(d_ptr->margins).latitude;
}

void QMapboxGL::setLatitude(double latitude_)
{
    d_ptr->mapObj->setLatLng(mbgl::LatLng { latitude_, longitude() }, d_ptr->margins);
}

double QMapboxGL::longitude() const
{
    return d_ptr->mapObj->getLatLng(d_ptr->margins).longitude;
}

void QMapboxGL::setLongitude(double longitude_)
{
    d_ptr->mapObj->setLatLng(mbgl::LatLng { latitude(), longitude_ }, d_ptr->margins);
}

double QMapboxGL::scale() const
{
    return d_ptr->mapObj->getScale();
}

void QMapboxGL::setScale(double scale_, const QPointF &center)
{
    d_ptr->mapObj->setScale(scale_, mbgl::ScreenCoordinate { center.x(), center.y() });
}

double QMapboxGL::zoom() const
{
    return d_ptr->mapObj->getZoom();
}

void QMapboxGL::setZoom(double zoom_)
{
    d_ptr->mapObj->setZoom(zoom_, d_ptr->margins);
}

double QMapboxGL::minimumZoom() const
{
    return d_ptr->mapObj->getMinZoom();
}

double QMapboxGL::maximumZoom() const
{
    return d_ptr->mapObj->getMaxZoom();
}

Coordinate QMapboxGL::coordinate() const
{
    const mbgl::LatLng& latLng = d_ptr->mapObj->getLatLng(d_ptr->margins);
    return Coordinate(latLng.latitude, latLng.longitude);
}

void QMapboxGL::setCoordinate(const Coordinate &coordinate_)
{
    d_ptr->mapObj->setLatLng(mbgl::LatLng { coordinate_.first, coordinate_.second }, d_ptr->margins);
}

void QMapboxGL::setCoordinateZoom(const Coordinate &coordinate_, double zoom_)
{
    d_ptr->mapObj->setLatLngZoom(
            mbgl::LatLng { coordinate_.first, coordinate_.second }, zoom_, d_ptr->margins);
}

void QMapboxGL::jumpTo(const CameraOptions& camera)
{
    mbgl::CameraOptions mbglCamera;
    if (camera.center.isValid()) {
        const Coordinate center = camera.center.value<Coordinate>();
        mbglCamera.center = mbgl::LatLng { center.first, center.second };
    }
    if (camera.anchor.isValid()) {
        const QPointF anchor = camera.anchor.value<QPointF>();
        mbglCamera.anchor = mbgl::ScreenCoordinate { anchor.x(), anchor.y() };
    }
    if (camera.zoom.isValid()) {
        mbglCamera.zoom = camera.zoom.value<double>();
    }
    if (camera.angle.isValid()) {
        mbglCamera.angle = -camera.angle.value<double>() * mbgl::util::DEG2RAD;
    }
    if (camera.pitch.isValid()) {
        mbglCamera.pitch = camera.pitch.value<double>() * mbgl::util::DEG2RAD;
    }

    mbglCamera.padding = d_ptr->margins;

    d_ptr->mapObj->jumpTo(mbglCamera);
}

double QMapboxGL::bearing() const
{
    return d_ptr->mapObj->getBearing();
}

void QMapboxGL::setBearing(double degrees)
{
    d_ptr->mapObj->setBearing(degrees, d_ptr->margins);
}

void QMapboxGL::setBearing(double degrees, const QPointF &center)
{
    d_ptr->mapObj->setBearing(degrees, mbgl::ScreenCoordinate { center.x(), center.y() });
}

double QMapboxGL::pitch() const
{
    return d_ptr->mapObj->getPitch();
}

void QMapboxGL::setPitch(double pitch_)
{
    d_ptr->mapObj->setPitch(pitch_);
}

QMapboxGL::NorthOrientation QMapboxGL::northOrientation() const
{
    return static_cast<QMapboxGL::NorthOrientation>(d_ptr->mapObj->getNorthOrientation());
}

void QMapboxGL::setNorthOrientation(NorthOrientation orientation)
{
    d_ptr->mapObj->setNorthOrientation(static_cast<mbgl::NorthOrientation>(orientation));
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
    d_ptr->mapObj->setClasses(fromQStringList(classNames));
}

QStringList QMapboxGL::getClasses() const
{
    QStringList classNames;
    for (const std::string &mbglClass : d_ptr->mapObj->getClasses()) {
        classNames << QString::fromStdString(mbglClass);
    }
    return classNames;
}

QMapbox::TransitionOptions QMapboxGL::getTransitionOptions() const {
    return toQMapboxTransitionOptions(d_ptr->mapObj->getTransitionOptions());
}

void QMapboxGL::setTransitionOptions(const QMapbox::TransitionOptions &options) {
    d_ptr->mapObj->setTransitionOptions(fromQMapboxTransitionOptions(options));
}

mbgl::Annotation fromPointAnnotation(const PointAnnotation &pointAnnotation) {
    const Coordinate &coordinate = pointAnnotation.first;
    const QString &icon = pointAnnotation.second;
    return mbgl::SymbolAnnotation { mbgl::Point<double> { coordinate.second, coordinate.first }, icon.toStdString() };
}

AnnotationID QMapboxGL::addPointAnnotation(const PointAnnotation &pointAnnotation)
{
    return d_ptr->mapObj->addAnnotation(fromPointAnnotation(pointAnnotation));
}

void QMapboxGL::updatePointAnnotation(AnnotationID id, const PointAnnotation &pointAnnotation)
{
    d_ptr->mapObj->updateAnnotation(id, fromPointAnnotation(pointAnnotation));
}

AnnotationID QMapboxGL::addShapeAnnotation(const ShapeAnnotation &shapeAnnotation)
{
    return d_ptr->mapObj->addAnnotation(fromQMapboxGLShapeAnnotation(shapeAnnotation));
}

void QMapboxGL::removeAnnotation(AnnotationID annotationID)
{
    d_ptr->mapObj->removeAnnotation(annotationID);
}

void QMapboxGL::setLayoutProperty(const QString& layer_, const QString& property, const QVariant& value)
{
    using namespace mbgl::style;

    Layer* layer = d_ptr->mapObj->getLayer(layer_.toStdString());
    if (!layer) {
        qWarning() << "Layer not found:" << layer_;
        return;
    }

    if (conversion::setLayoutProperty(*layer, property.toStdString(), value)) {
        qWarning() << "Error setting layout property:" << layer_ << "-" << property;
        return;
    }
}

void QMapboxGL::setPaintProperty(const QString& layer_, const QString& property, const QVariant& value, const QString& klass_)
{
    using namespace mbgl::style;

    Layer* layer = d_ptr->mapObj->getLayer(layer_.toStdString());
    if (!layer) {
        qWarning() << "Layer not found:" << layer_;
        return;
    }

    mbgl::optional<std::string> klass;
    if (!klass_.isEmpty()) {
        klass = klass_.toStdString();
    }

    if (conversion::setPaintProperty(*layer, property.toStdString(), value, klass)) {
        qWarning() << "Error setting paint property:" << layer_ << "-" << property;
        return;
    }
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

void QMapboxGL::scaleBy(double scale_, const QPointF &center) {
    d_ptr->mapObj->scaleBy(scale_, mbgl::ScreenCoordinate { center.x(), center.y() });
}

void QMapboxGL::rotateBy(const QPointF &first, const QPointF &second)
{
    d_ptr->mapObj->rotateBy(
            mbgl::ScreenCoordinate { first.x(), first.y() },
            mbgl::ScreenCoordinate { second.x(), second.y() });
}

void QMapboxGL::resize(const QSize& size)
{
    QSize converted = size / d_ptr->getPixelRatio();
    if (d_ptr->size == converted) return;

    glViewport(0, 0, converted.width(), converted.height());

    d_ptr->size = converted;
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

    d_ptr->mapObj->addAnnotationIcon(
        name.toStdString(),
        std::make_shared<mbgl::SpriteImage>(
            mbgl::PremultipliedImage{ static_cast<uint16_t>(swapped.width()),
                                      static_cast<uint16_t>(swapped.height()), std::move(img) },
            1.0));
}

QPointF QMapboxGL::pixelForCoordinate(const Coordinate &coordinate_) const
{
    const mbgl::ScreenCoordinate pixel =
        d_ptr->mapObj->pixelForLatLng(mbgl::LatLng { coordinate_.first, coordinate_.second });

    return QPointF(pixel.x, pixel.y);
}

Coordinate QMapboxGL::coordinateForPixel(const QPointF &pixel) const
{
    const mbgl::LatLng latLng =
        d_ptr->mapObj->latLngForPixel(mbgl::ScreenCoordinate { pixel.x(), pixel.y() });

    return Coordinate(latLng.latitude, latLng.longitude);
}

CoordinateZoom QMapboxGL::coordinateZoomForBounds(const Coordinate &sw, Coordinate &ne) const
{
    auto bounds = mbgl::LatLngBounds::hull(mbgl::LatLng { sw.first, sw.second }, mbgl::LatLng { ne.first, ne.second });
    mbgl::CameraOptions camera = d_ptr->mapObj->cameraForLatLngBounds(bounds, d_ptr->margins);

    return {{ (*camera.center).latitude, (*camera.center).longitude }, *camera.zoom };
}

CoordinateZoom QMapboxGL::coordinateZoomForBounds(const Coordinate &sw, Coordinate &ne,
    double newBearing, double newPitch)
{
    // FIXME: mbgl::Map::cameraForLatLngBounds should
    // take bearing and pitch as input too, so this
    // hack won't be needed.
    double currentBearing = bearing();
    double currentPitch = pitch();

    setBearing(newBearing);
    setPitch(newPitch);

    auto bounds = mbgl::LatLngBounds::hull(mbgl::LatLng { sw.first, sw.second }, mbgl::LatLng { ne.first, ne.second });
    mbgl::CameraOptions camera = d_ptr->mapObj->cameraForLatLngBounds(bounds, d_ptr->margins);

    setBearing(currentBearing);
    setPitch(currentPitch);

    return {{ (*camera.center).latitude, (*camera.center).longitude }, *camera.zoom };
}

void QMapboxGL::setMargins(const QMargins &margins_)
{
    d_ptr->margins = {
        static_cast<double>(margins_.top()),
        static_cast<double>(margins_.left()),
        static_cast<double>(margins_.bottom()),
        static_cast<double>(margins_.right())
    };
}

QMargins QMapboxGL::margins() const
{
    return QMargins(
        d_ptr->margins.left,
        d_ptr->margins.top,
        d_ptr->margins.right,
        d_ptr->margins.bottom
    );
}

void QMapboxGL::addSource(const QString &sourceID, const QVariantMap &params)
{
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    Result<std::unique_ptr<Source>> source = convert<std::unique_ptr<Source>>(QVariant(params), sourceID.toStdString());
    if (!source) {
        qWarning() << "Unable to add source:" << source.error().message.c_str();
        return;
    }

    d_ptr->mapObj->addSource(std::move(*source));
}

void QMapboxGL::removeSource(const QString& sourceID)
{
    d_ptr->mapObj->removeSource(sourceID.toStdString());
}

void QMapboxGL::addCustomLayer(const QString &id,
        QMapbox::CustomLayerInitializeFunction initFn,
        QMapbox::CustomLayerRenderFunction renderFn,
        QMapbox::CustomLayerDeinitializeFunction deinitFn,
        void *context_,
        char *before)
{
    d_ptr->mapObj->addLayer(std::make_unique<mbgl::style::CustomLayer>(
            id.toStdString(),
            reinterpret_cast<mbgl::style::CustomLayerInitializeFunction>(initFn),
            // This cast is safe as long as both mbgl:: and QMapbox::
            // CustomLayerRenderParameters members remains the same.
            (mbgl::style::CustomLayerRenderFunction)renderFn,
            reinterpret_cast<mbgl::style::CustomLayerDeinitializeFunction>(deinitFn),
            context_),
            before ? mbgl::optional<std::string>(before) : mbgl::optional<std::string>());
}

void QMapboxGL::addLayer(const QVariantMap &params)
{
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    Result<std::unique_ptr<Layer>> layer = convert<std::unique_ptr<Layer>>(QVariant(params));
    if (!layer) {
        qWarning() << "Unable to add layer:" << layer.error().message.c_str();
        return;
    }

    d_ptr->mapObj->addLayer(std::move(*layer));
}

void QMapboxGL::removeLayer(const QString& id)
{
    d_ptr->mapObj->removeLayer(id.toStdString());
}

void QMapboxGL::setFilter(const QString& layer_, const QVariant& filter_)
{
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    Layer* layer = d_ptr->mapObj->getLayer(layer_.toStdString());
    if (!layer) {
        qWarning() << "Layer not found:" << layer_;
        return;
    }

    Filter filter;

    Result<Filter> converted = convert<Filter>(filter_);
    if (!converted) {
        qWarning() << "Error parsing filter:" << converted.error().message.c_str();
        return;
    }
    filter = std::move(*converted);

    if (layer->is<FillLayer>()) {
        layer->as<FillLayer>()->setFilter(filter);
        return;
    }
    if (layer->is<LineLayer>()) {
        layer->as<LineLayer>()->setFilter(filter);
        return;
    }
    if (layer->is<SymbolLayer>()) {
        layer->as<SymbolLayer>()->setFilter(filter);
        return;
    }
    if (layer->is<CircleLayer>()) {
        layer->as<CircleLayer>()->setFilter(filter);
        return;
    }

    qWarning() << "Layer doesn't support filters";
}

void QMapboxGL::render()
{
    d_ptr->dirty = false;
    d_ptr->mapObj->render();
}

void QMapboxGL::connectionEstablished()
{
    d_ptr->connectionEstablished();
}

QMapboxGLPrivate::QMapboxGLPrivate(QMapboxGL *q, const QMapboxGLSettings &settings)
    : QObject(q)
    , q_ptr(q)
    , fileSourceObj(std::make_unique<mbgl::DefaultFileSource>(
        settings.cacheDatabasePath().toStdString(),
        settings.assetPath().toStdString(),
        settings.cacheDatabaseMaximumSize()))
    , mapObj(std::make_unique<mbgl::Map>(
        *this, *fileSourceObj,
        static_cast<mbgl::MapMode>(settings.mapMode()),
        static_cast<mbgl::GLContextMode>(settings.contextMode()),
        static_cast<mbgl::ConstrainMode>(settings.constrainMode()),
        static_cast<mbgl::ViewportMode>(settings.viewportMode())))
{
    qRegisterMetaType<QMapboxGL::MapChange>("QMapboxGL::MapChange");

    fileSourceObj->setAccessToken(settings.accessToken().toStdString());
    connect(this, SIGNAL(needsRendering()), q_ptr, SIGNAL(needsRendering()), Qt::QueuedConnection);
    connect(this, SIGNAL(mapChanged(QMapboxGL::MapChange)), q_ptr, SIGNAL(mapChanged(QMapboxGL::MapChange)), Qt::QueuedConnection);
}

QMapboxGLPrivate::~QMapboxGLPrivate()
{
}

float QMapboxGLPrivate::getPixelRatio() const
{
#if QT_VERSION >= 0x050000
    // QWindow is the most reliable pixel ratio because QGuiApplication returns
    // the maximum pixel ratio of all available QScreen objects - this is not
    // valid for cases e.g. where two or more QScreen objects with different
    // pixel ratios are present and the window shows on the screen with lower
    // pixel ratio.
    static const float pixelRatio = QGuiApplication::allWindows().first()->devicePixelRatio();
#else
    static const float pixelRatio = 1.0;
#endif
    return pixelRatio;
}

std::array<uint16_t, 2> QMapboxGLPrivate::getSize() const
{
    return {{ static_cast<uint16_t>(size.width()), static_cast<uint16_t>(size.height()) }};
}

std::array<uint16_t, 2> QMapboxGLPrivate::getFramebufferSize() const
{
    return {{ static_cast<uint16_t>(size.width() * getPixelRatio()),
              static_cast<uint16_t>(size.height() * getPixelRatio()) }};
}

void QMapboxGLPrivate::invalidate()
{
    if (!dirty) {
        emit needsRendering();
        dirty = true;
    }
}

void QMapboxGLPrivate::notifyMapChange(mbgl::MapChange change)
{
    emit mapChanged(static_cast<QMapboxGL::MapChange>(change));
}

void QMapboxGLPrivate::connectionEstablished()
{
    mbgl::NetworkStatus::Reachable();
}
