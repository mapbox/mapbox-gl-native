#include "qmapboxgl.hpp"
#include "qmapboxgl_p.hpp"

#include "qt_conversion.hpp"
#include "qt_geojson.hpp"

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/backend_scope.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/shared_thread_pool.hpp>
#include <mbgl/util/traits.hpp>

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <QWindow>
#include <QOpenGLContext>
#else
#include <QCoreApplication>
#include <QGLContext>
#endif

#include <QDebug>
#include <QImage>
#include <QMargins>
#include <QString>
#include <QStringList>
#include <QThreadStorage>

#include <memory>

using namespace QMapbox;

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

namespace {

QThreadStorage<std::shared_ptr<mbgl::util::RunLoop>> loop;

// Conversion helper functions.

auto fromQStringList(const QStringList &list)
{
    std::vector<std::string> strings;
    strings.reserve(list.size());
    for (const QString &string : list) {
        strings.push_back(string.toStdString());
    }
    return strings;
}

mbgl::Size sanitizedSize(const QSize& size) {
    return mbgl::Size {
        mbgl::util::max(0u, static_cast<uint32_t>(size.width())),
        mbgl::util::max(0u, static_cast<uint32_t>(size.height())),
    };
};

std::unique_ptr<mbgl::style::Image> toStyleImage(const QImage &sprite) {
    const QImage swapped = sprite
        .rgbSwapped()
        .convertToFormat(QImage::Format_ARGB32_Premultiplied);

    auto img = std::make_unique<uint8_t[]>(swapped.byteCount());
    memcpy(img.get(), swapped.constBits(), swapped.byteCount());

    return std::make_unique<mbgl::style::Image>(
        mbgl::PremultipliedImage(
            { static_cast<uint32_t>(swapped.width()), static_cast<uint32_t>(swapped.height()) },
            std::move(img)),
        1.0);
}

} // namespace

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

    This enum sets the expectations for the OpenGL state.

    \value UniqueGLContext  The OpenGL context is only used by QMapboxGL, so it is not
    reset before each rendering. Use this mode if the intention is to only draw a
    fullscreen map.

    \value SharedGLContext  The OpenGL context is shared and the state will be restored
    before rendering. This mode is safer when OpenGL calls are performed prior of after
    we call QMapboxGL::render for rendering a map.

    \sa contextMode()
*/

/*!
    \enum QMapboxGLSettings::ConstrainMode

    This enum determines if the map wraps.

    \value NoConstrain              The map will wrap on the horizontal axis. Since it doesn't
    make sense to wrap on the vertical axis in a Web Mercator projection, the map will scroll
    and show some empty space.

    \value ConstrainHeightOnly      The map will wrap around the horizontal axis, like a spinning
    globe. This is the recommended constrain mode.

    \value ConstrainWidthAndHeight  The map won't wrap and panning is restricted to the boundaries
    of the map.

    \sa constrainMode()
*/

/*!
    \enum QMapboxGLSettings::ViewportMode

    This enum flips the map vertically.

    \value DefaultViewport  Native orientation.

    \value FlippedYViewport  Mirrored vertically.

    \sa viewportMode()
*/

/*!
    Constructs a QMapboxGLSettings object with the default values. The default
    configuration is valid for initializing a QMapboxGL.
*/
QMapboxGLSettings::QMapboxGLSettings()
    : m_contextMode(QMapboxGLSettings::SharedGLContext)
    , m_constrainMode(QMapboxGLSettings::ConstrainHeightOnly)
    , m_viewportMode(QMapboxGLSettings::DefaultViewport)
    , m_cacheMaximumSize(mbgl::util::DEFAULT_MAX_CACHE_SIZE)
    , m_cacheDatabasePath(":memory:")
    , m_assetPath(QCoreApplication::applicationDirPath())
    , m_accessToken(qgetenv("MAPBOX_ACCESS_TOKEN"))
    , m_apiBaseUrl(mbgl::util::API_BASE_URL)
{
}

/*!
    Returns the OpenGL context mode. This is specially important when mixing
    with other OpenGL draw calls.

    By default, it is set to QMapboxGLSettings::SharedGLContext.
*/
QMapboxGLSettings::GLContextMode QMapboxGLSettings::contextMode() const
{
    return m_contextMode;
}

/*!
    Sets the OpenGL context \a mode.
*/
void QMapboxGLSettings::setContextMode(GLContextMode mode)
{
    m_contextMode = mode;
}

/*!
    Returns the constrain mode. This is used to limit the map to wrap
    around the globe horizontally.

    By default, it is set to QMapboxGLSettings::ConstrainHeightOnly.
*/
QMapboxGLSettings::ConstrainMode QMapboxGLSettings::constrainMode() const
{
    return m_constrainMode;
}

/*!
    Sets the map constrain \a mode.
*/
void QMapboxGLSettings::setConstrainMode(ConstrainMode mode)
{
    m_constrainMode = mode;
}

/*!
    Returns the viewport mode. This is used to flip the vertical
    orientation of the map as some devices may use inverted orientation.

    By default, it is set to QMapboxGLSettings::DefaultViewport.
*/
QMapboxGLSettings::ViewportMode QMapboxGLSettings::viewportMode() const
{
    return m_viewportMode;
}

/*!
    Sets the viewport \a mode.
*/
void QMapboxGLSettings::setViewportMode(ViewportMode mode)
{
    m_viewportMode = mode;
}

/*!
    Returns the cache database maximum hard size in bytes. The database
    will grow until the limit is reached. Setting a maximum size smaller
    than the current size of an existing database results in undefined
    behavior

    By default, it is set to 50 MB.
*/
unsigned QMapboxGLSettings::cacheDatabaseMaximumSize() const
{
    return m_cacheMaximumSize;
}

/*!
    Returns the maximum allowed cache database \a size in bytes.
*/
void QMapboxGLSettings::setCacheDatabaseMaximumSize(unsigned size)
{
    m_cacheMaximumSize = size;
}

/*!
    Returns the cache database path. The cache is used for storing
    recently used resources like tiles and also an offline tile database
    pre-populated by the
    \l {https://github.com/mapbox/mapbox-gl-native/blob/master/bin/offline.sh}
    {Offline Tool}.

    By default, it is set to \c :memory: meaning it will create an in-memory
    cache instead of a file on disk.
*/
QString QMapboxGLSettings::cacheDatabasePath() const
{
    return m_cacheDatabasePath;
}

/*!
    Sets the cache database \a path.

    Setting the \a path to \c :memory: will create an in-memory cache.
*/
void QMapboxGLSettings::setCacheDatabasePath(const QString &path)
{
    m_cacheDatabasePath = path;
}

/*!
    Returns the asset path, which is the root directory from where
    the \c asset:// scheme gets resolved in a style. \c asset:// can be used
    for loading a resource from the disk in a style rather than fetching
    it from the network.

    By default, it is set to the value returned by QCoreApplication::applicationDirPath().
*/
QString QMapboxGLSettings::assetPath() const
{
    return m_assetPath;
}

/*!
    Sets the asset \a path.
*/
void QMapboxGLSettings::setAssetPath(const QString &path)
{
    m_assetPath = path;
}

/*!
    Returns the access token.

    By default, it is taken from the environment variable \c MAPBOX_ACCESS_TOKEN
    or empty if the variable is not set.
*/
QString QMapboxGLSettings::accessToken() const {
    return m_accessToken;
}

/*!
    Sets the access \a token.

    Mapbox-hosted vector tiles and styles require an API
    \l {https://www.mapbox.com/help/define-access-token/}{access token}, which you
    can obtain from the \l {https://www.mapbox.com/studio/account/tokens/}
    {Mapbox account page}. Access tokens associate requests to Mapbox's vector tile
    and style APIs with your Mapbox account. They also deter other developers from
    using your styles without your permission.
*/
void QMapboxGLSettings::setAccessToken(const QString &token)
{
    m_accessToken = token;
}

/*!
    Returns the API base URL.
*/
QString QMapboxGLSettings::apiBaseUrl() const
{
    return m_apiBaseUrl;
}

/*!
    Sets the API base \a url.

    The API base URL is the URL that the \b "mapbox://" protocol will
    be resolved to. It defaults to "https://api.mapbox.com" but can be
    changed, for instance, to a tile cache server address.
*/
void QMapboxGLSettings::setApiBaseUrl(const QString& url)
{
    m_apiBaseUrl = url;
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
    the same thread as the thread where the OpenGL context lives.

    \since 4.7
*/

/*!
    \enum QMapboxGL::MapChange

    This enum represents the last changed occurred to the map state.

    \value MapChangeRegionWillChange                      A region of the map will change, like
    when resizing the map.

    \value MapChangeRegionWillChangeAnimated              Not in use by QMapboxGL.

    \value MapChangeRegionIsChanging                      A region of the map is changing.

    \value MapChangeRegionDidChange                       A region of the map finished changing.

    \value MapChangeRegionDidChangeAnimated               Not in use by QMapboxGL.

    \value MapChangeWillStartLoadingMap                   The map is getting loaded. This state
    is set only once right after QMapboxGL is created and a style is set.

    \value MapChangeDidFinishLoadingMap                   All the resources were loaded and parsed
    and the map is fully rendered. After this state the mapChanged() signal won't fire again unless
    the is some client side interaction with the map or a tile expires, causing a new resource
    to be requested from the network.

    \value MapChangeDidFailLoadingMap                     An error occurred when loading the map.

    \value MapChangeWillStartRenderingFrame               Just before rendering the frame. This
    is the state of the map just after calling render() and might happened many times before
    the map is fully loaded.

    \value MapChangeDidFinishRenderingFrame               The current frame was rendered but was
    left in a partial state. Some parts of the map might be missing because have not arrived
    from the network or are being parsed.

    \value MapChangeDidFinishRenderingFrameFullyRendered  The current frame was fully rendered.

    \value MapChangeWillStartRenderingMap                 Set once when the map is about to get
    rendered for the first time.

    \value MapChangeDidFinishRenderingMap                 Not in use by QMapboxGL.

    \value MapChangeDidFinishRenderingMapFullyRendered    Map is fully loaded and rendered.

    \value MapChangeDidFinishLoadingStyle                 The style was loaded.

    \value MapChangeSourceDidChange                       A source has changed.

    \sa mapChanged()
*/

/*!
    \enum QMapboxGL::NorthOrientation

    This enum sets the orientation of the north bearing. It will directly affect bearing when
    resetting the north (i.e. setting bearing to 0).

    \value NorthUpwards     The north is pointing up in the map. This is usually how maps are oriented.

    \value NorthRightwards  The north is pointing right.

    \value NorthDownwards   The north is pointing down.

    \value NorthLeftwards   The north is pointing left.

    \sa northOrientation()
    \sa bearing()
*/

/*!
    Constructs a QMapboxGL object with \a settings and sets \a parent as the parent
    object. The \a settings cannot be changed after the object is constructed. The
    \a size represents the size of the viewport and the \a pixelRatio the initial pixel
    density of the screen.
*/
QMapboxGL::QMapboxGL(QObject *parent, const QMapboxGLSettings &settings, const QSize& size, qreal pixelRatio)
    : QObject(parent)
{
    assert(!size.isEmpty());

    // Multiple QMapboxGL running on the same thread
    // will share the same mbgl::util::RunLoop
    if (!loop.hasLocalData()) {
        loop.setLocalData(std::make_shared<mbgl::util::RunLoop>());
    }

    d_ptr = new QMapboxGLPrivate(this, settings, size, pixelRatio);
}

/*!
    Destroys this QMapboxGL.
*/
QMapboxGL::~QMapboxGL()
{
    delete d_ptr;
}

/*!
    Cycles through several debug options like showing the tile borders,
    tile numbers, expiration time and wireframe.
*/
void QMapboxGL::cycleDebugOptions()
{
    d_ptr->mapObj->cycleDebugOptions();
}

/*!
    \property QMapboxGL::styleJson
    \brief the map style JSON.

    Sets a new \a style from a JSON that must conform to the
    \l {https://www.mapbox.com/mapbox-gl-style-spec/}
    {Mapbox style specification}.

    \note In case of a invalid style it will trigger a mapChanged
    signal with QMapboxGL::MapChangeDidFailLoadingMap as argument.
*/
QString QMapboxGL::styleJson() const
{
    return QString::fromStdString(d_ptr->mapObj->getStyleJSON());
}

void QMapboxGL::setStyleJson(const QString &style)
{
    d_ptr->mapObj->setStyleJSON(style.toStdString());
}

/*!
    \property QMapboxGL::styleUrl
    \brief the map style URL.

    Sets a URL for fetching a JSON that will be later fed to
    setStyleJson. URLs using the Mapbox scheme (\a mapbox://) are
    also accepted and translated automatically to an actual HTTPS
    request.

    The Mapbox scheme is not enforced and a style can be fetched
    from anything that QNetworkAccessManager can handle.

    \note In case of a invalid style it will trigger a mapChanged
    signal with QMapboxGL::MapChangeDidFailLoadingMap as argument.
*/
QString QMapboxGL::styleUrl() const
{
    return QString::fromStdString(d_ptr->mapObj->getStyleURL());
}

void QMapboxGL::setStyleUrl(const QString &url)
{
    d_ptr->mapObj->setStyleURL(url.toStdString());
}

/*!
    \property QMapboxGL::latitude
    \brief the map's current latitude in degrees.

    Setting a latitude doesn't necessarily mean it will be accepted since QMapboxGL
    might constrain it within the limits of the Web Mercator projection.
*/
double QMapboxGL::latitude() const
{
    return d_ptr->mapObj->getLatLng(d_ptr->margins).latitude();
}

void QMapboxGL::setLatitude(double latitude_)
{
    d_ptr->mapObj->setLatLng(mbgl::LatLng { latitude_, longitude() }, d_ptr->margins);
}

/*!
    \property QMapboxGL::longitude
    \brief the map current longitude in degrees.

    Setting a longitude beyond the limits of the Web Mercator projection will make
    the map wrap. As an example, setting the longitude to 360 is effectively the same
    as setting it to 0.
*/
double QMapboxGL::longitude() const
{
    return d_ptr->mapObj->getLatLng(d_ptr->margins).longitude();
}

void QMapboxGL::setLongitude(double longitude_)
{
    d_ptr->mapObj->setLatLng(mbgl::LatLng { latitude(), longitude_ }, d_ptr->margins);
}

/*!
    \property QMapboxGL::scale
    \brief the map scale factor.

    This property is used to zoom the map. When \a center is defined, the map will
    scale in the direction of the center pixel coordinates. The \a center will remain
    at the same pixel coordinate after scaling as before calling this method.

    \note This function could be used for implementing a pinch gesture or zooming
    by using the mouse scroll wheel.

    \sa zoom()
*/
double QMapboxGL::scale() const
{
    return std::pow(2.0, d_ptr->mapObj->getZoom());
}

void QMapboxGL::setScale(double scale_, const QPointF &center)
{
    d_ptr->mapObj->setZoom(std::log2(scale_), mbgl::ScreenCoordinate { center.x(), center.y() });
}

/*!
    \property QMapboxGL::zoom
    \brief the map zoom factor.

    This property is used to zoom the map. When \a center is defined, the map will
    zoom in the direction of the center. This function could be used for implementing
    a pinch gesture or zooming by using the mouse scroll wheel.

    \sa scale()
*/
double QMapboxGL::zoom() const
{
    return d_ptr->mapObj->getZoom();
}

void QMapboxGL::setZoom(double zoom_)
{
    d_ptr->mapObj->setZoom(zoom_, d_ptr->margins);
}

/*!
    Returns the minimum zoom level allowed for the map.

    \sa maximumZoom()
*/
double QMapboxGL::minimumZoom() const
{
    return d_ptr->mapObj->getMinZoom();
}

/*!
    Returns the maximum zoom level allowed for the map.

    \sa minimumZoom()
*/
double QMapboxGL::maximumZoom() const
{
    return d_ptr->mapObj->getMaxZoom();
}

/*!
    \property QMapboxGL::coordinate
    \brief the map center \a coordinate.

    Centers the map at a geographic coordinate respecting the margins, if set.

    \sa margins()
*/
Coordinate QMapboxGL::coordinate() const
{
    const mbgl::LatLng& latLng = d_ptr->mapObj->getLatLng(d_ptr->margins);
    return Coordinate(latLng.latitude(), latLng.longitude());
}

void QMapboxGL::setCoordinate(const QMapbox::Coordinate &coordinate_)
{
    d_ptr->mapObj->setLatLng(mbgl::LatLng { coordinate_.first, coordinate_.second }, d_ptr->margins);
}

/*!
    \fn QMapboxGL::setCoordinateZoom(const QMapbox::Coordinate &coordinate, double zoom)

    Convenience method for setting the \a coordinate and \a zoom simultaneously.

    \note Setting \a coordinate and \a zoom at once is more efficient than doing
    it in two steps.

    \sa zoom()
    \sa coordinate()
*/
void QMapboxGL::setCoordinateZoom(const QMapbox::Coordinate &coordinate_, double zoom_)
{
    d_ptr->mapObj->setLatLngZoom(
            mbgl::LatLng { coordinate_.first, coordinate_.second }, zoom_, d_ptr->margins);
}

/*!
    Atomically jumps to the \a camera options.
*/
void QMapboxGL::jumpTo(const QMapboxGLCameraOptions& camera)
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

/*!
    \property QMapboxGL::bearing
    \brief the map bearing in degrees.

    Set the angle in degrees. Negative values and values over 360 are
    valid and will wrap. The direction of the rotation is counterclockwise.

    When \a center is defined, the map will rotate around the center pixel coordinate
    respecting the margins if defined.

    \sa margins()
*/
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

/*!
    \property QMapboxGL::pitch
    \brief the map pitch in degrees.

    Pitch toward the horizon measured in degrees, with 0 resulting in a
    two-dimensional map. It will be constrained at 60 degrees.

    \sa margins()
*/
double QMapboxGL::pitch() const
{
    return d_ptr->mapObj->getPitch();
}

void QMapboxGL::setPitch(double pitch_)
{
    d_ptr->mapObj->setPitch(pitch_);
}

/*!
    Returns the north orientation mode.
*/
QMapboxGL::NorthOrientation QMapboxGL::northOrientation() const
{
    return static_cast<QMapboxGL::NorthOrientation>(d_ptr->mapObj->getNorthOrientation());
}

/*!
    Sets the north orientation mode to \a orientation.
*/
void QMapboxGL::setNorthOrientation(NorthOrientation orientation)
{
    d_ptr->mapObj->setNorthOrientation(static_cast<mbgl::NorthOrientation>(orientation));
}

/*!
    Tells the map rendering engine that there is currently a gesture in \a progress. This
    affects how the map renders labels, as it will use different texture filters if a gesture
    is ongoing.
*/
void QMapboxGL::setGestureInProgress(bool progress)
{
    d_ptr->mapObj->setGestureInProgress(progress);
}

/*!
    Adds an \a className to the list of active classes. Layers tagged with a certain class
    will only be active when the class is added.

    This was removed from the \l {https://www.mapbox.com/mapbox-gl-style-spec/#layer-paint.*}
    {Mapbox style specification} and should no longer be used.

    \deprecated
    \sa removeClass()
*/
void QMapboxGL::addClass(const QString &className)
{
    d_ptr->mapObj->addClass(className.toStdString());
}

/*!
    Removes a \a className.

    \deprecated
    \sa addClass()
*/
void QMapboxGL::removeClass(const QString &className)
{
    d_ptr->mapObj->removeClass(className.toStdString());
}

/*!
    Returns true when \a className is active, false otherwise.

    \deprecated
    \sa addClass()
*/
bool QMapboxGL::hasClass(const QString &className) const
{
    return d_ptr->mapObj->hasClass(className.toStdString());
}

/*!
    Bulk adds a list of \a classNames.

    \deprecated
    \sa addClass()
*/
void QMapboxGL::setClasses(const QStringList &classNames)
{
    d_ptr->mapObj->setClasses(fromQStringList(classNames));
}

/*!
    Returns a list of active classes.

    \deprecated
    \sa setClasses()
*/
QStringList QMapboxGL::getClasses() const
{
    QStringList classNames;
    for (const std::string &mbglClass : d_ptr->mapObj->getClasses()) {
        classNames << QString::fromStdString(mbglClass);
    }
    return classNames;
}

/*!
    Sets the \a duration and \a delay of style class transitions. Style property
    values transition to new values with animation when a new class is set.

    \deprecated
    \sa addClass()
*/
void QMapboxGL::setTransitionOptions(qint64 duration, qint64 delay) {
    static auto convert = [](qint64 value) -> mbgl::optional<mbgl::Duration> {
        return std::chrono::duration_cast<mbgl::Duration>(mbgl::Milliseconds(value));
    };

    d_ptr->mapObj->setTransitionOptions(mbgl::style::TransitionOptions{ convert(duration), convert(delay) });
}

mbgl::Annotation asMapboxGLAnnotation(const QMapbox::Annotation & annotation) {
    auto asMapboxGLGeometry = [](const QMapbox::ShapeAnnotationGeometry &geometry) {
        mbgl::ShapeAnnotationGeometry result;
        switch (geometry.type) {
        case QMapbox::ShapeAnnotationGeometry::LineStringType:
            result = { asMapboxGLLineString(geometry.geometry.first().first()) };
            break;
        case QMapbox::ShapeAnnotationGeometry::PolygonType:
            result = { asMapboxGLPolygon(geometry.geometry.first()) };
            break;
        case QMapbox::ShapeAnnotationGeometry::MultiLineStringType:
            result = { asMapboxGLMultiLineString(geometry.geometry.first()) };
            break;
        case QMapbox::ShapeAnnotationGeometry::MultiPolygonType:
            result = { asMapboxGLMultiPolygon(geometry.geometry) };
            break;
        }
        return result;
    };

    if (annotation.canConvert<QMapbox::SymbolAnnotation>()) {
        QMapbox::SymbolAnnotation symbolAnnotation = annotation.value<QMapbox::SymbolAnnotation>();
        QMapbox::Coordinate& pair = symbolAnnotation.geometry;
        return mbgl::SymbolAnnotation { mbgl::Point<double> { pair.second, pair.first }, symbolAnnotation.icon.toStdString() };
    } else if (annotation.canConvert<QMapbox::LineAnnotation>()) {
        QMapbox::LineAnnotation lineAnnotation = annotation.value<QMapbox::LineAnnotation>();
        auto color = mbgl::Color::parse(lineAnnotation.color.name().toStdString());
        return mbgl::LineAnnotation { asMapboxGLGeometry(lineAnnotation.geometry), lineAnnotation.opacity, lineAnnotation.width, { *color } };
    } else if (annotation.canConvert<QMapbox::FillAnnotation>()) {
        QMapbox::FillAnnotation fillAnnotation = annotation.value<QMapbox::FillAnnotation>();
        auto color = mbgl::Color::parse(fillAnnotation.color.name().toStdString());
        if (fillAnnotation.outlineColor.canConvert<QColor>()) {
            auto outlineColor = mbgl::Color::parse(fillAnnotation.outlineColor.value<QColor>().name().toStdString());
            return mbgl::FillAnnotation { asMapboxGLGeometry(fillAnnotation.geometry), fillAnnotation.opacity, { *color }, { *outlineColor } };
        } else {
            return mbgl::FillAnnotation { asMapboxGLGeometry(fillAnnotation.geometry), fillAnnotation.opacity, { *color }, {} };
        }
    } else if (annotation.canConvert<QMapbox::StyleSourcedAnnotation>()) {
        QMapbox::StyleSourcedAnnotation styleSourcedAnnotation = annotation.value<QMapbox::StyleSourcedAnnotation>();
        return mbgl::StyleSourcedAnnotation { asMapboxGLGeometry(styleSourcedAnnotation.geometry), styleSourcedAnnotation.layerID.toStdString() };
    }

    qWarning() << "Unable to convert annotation:" << annotation;
    return {};
}

/*!
    Adds an \a annotation to the map.

    Returns the unique identifier for the new annotation.

    \sa addAnnotationIcon()
*/
QMapbox::AnnotationID QMapboxGL::addAnnotation(const QMapbox::Annotation &annotation)
{
    return d_ptr->mapObj->addAnnotation(asMapboxGLAnnotation(annotation));
}

/*!
    Updates an existing \a annotation referred by \a id.

    \sa addAnnotationIcon()
*/
void QMapboxGL::updateAnnotation(QMapbox::AnnotationID id, const QMapbox::Annotation &annotation)
{
    d_ptr->mapObj->updateAnnotation(id, asMapboxGLAnnotation(annotation));
}

/*!
    Removes an existing annotation referred by \a id.
*/
void QMapboxGL::removeAnnotation(QMapbox::AnnotationID id)
{
    d_ptr->mapObj->removeAnnotation(id);
}

/*!
    Sets a layout \a property \a value to an existing \a layer. The \a property string can be any
    as defined by the \l {https://www.mapbox.com/mapbox-gl-style-spec/} {Mapbox style specification}
    for layout properties.

    This example hides the layer \c route:

    \code
        map->setLayoutProperty("route", "visibility", "none");
    \endcode

    This table describes the mapping between \l {https://www.mapbox.com/mapbox-gl-style-spec/#types}
    {style types} and Qt types accepted by setLayoutProperty():

    \table
    \header
        \li Mapbox style type
        \li Qt type
    \row
        \li Enum
        \li QString
    \row
        \li String
        \li QString
    \row
        \li Boolean
        \li \c bool
    \row
        \li Number
        \li \c int, \c double or \c float
    \row
        \li Array
        \li QVariantList
    \endtable
*/
void QMapboxGL::setLayoutProperty(const QString& layer, const QString& property, const QVariant& value)
{
    using namespace mbgl::style;

    Layer* layer_ = d_ptr->mapObj->getLayer(layer.toStdString());
    if (!layer_) {
        qWarning() << "Layer not found:" << layer;
        return;
    }

    if (conversion::setLayoutProperty(*layer_, property.toStdString(), value)) {
        qWarning() << "Error setting layout property:" << layer << "-" << property;
        return;
    }
}

/*!
    Sets a paint \a property \a value to an existing \a layer. The \a property string can be any
    as defined by the \l {https://www.mapbox.com/mapbox-gl-style-spec/} {Mapbox style specification}
    for paint properties.

    The argument \a styleClass is deprecated and is used for defining the style class for the paint
    property.

    For paint properties that take a color as \a value, such as \c fill-color, a string such as
    \c blue can be passed or a QColor.

    \code
        map->setPaintProperty("route", "line-color", QColor("blue"));
    \endcode

    This table describes the mapping between \l {https://www.mapbox.com/mapbox-gl-style-spec/#types}
    {style types} and Qt types accepted by setPaintProperty():

    \table
    \header
        \li Mapbox style type
        \li Qt type
    \row
        \li Color
        \li QString or QColor
    \row
        \li Enum
        \li QString
    \row
        \li String
        \li QString
    \row
        \li Boolean
        \li \c bool
    \row
        \li Number
        \li \c int, \c double or \c float
    \row
        \li Array
        \li QVariantList
    \endtable

    If the style specification defines the property's type as \b Array, use a QVariantList. For
    example, the following code sets a \c route layer's \c line-dasharray property:

    \code
        QVariantList lineDashArray;
        lineDashArray.append(1);
        lineDashArray.append(2);

        map->setPaintProperty("route","line-dasharray", lineDashArray);
    \endcode
*/
void QMapboxGL::setPaintProperty(const QString& layer, const QString& property, const QVariant& value, const QString& styleClass)
{
    using namespace mbgl::style;

    Layer* layer_ = d_ptr->mapObj->getLayer(layer.toStdString());
    if (!layer_) {
        qWarning() << "Layer not found:" << layer;
        return;
    }

    mbgl::optional<std::string> klass;
    if (!styleClass.isEmpty()) {
        klass = styleClass.toStdString();
    }

    if (conversion::setPaintProperty(*layer_, property.toStdString(), value, klass)) {
        qWarning() << "Error setting paint property:" << layer << "-" << property;
        return;
    }
}

/*!
    Returns true when the map is completely rendered, false otherwise. A partially
    rendered map ranges from nothing rendered at all to only labels missing.
*/
bool QMapboxGL::isFullyLoaded() const
{
    return d_ptr->mapObj->isFullyLoaded();
}

/*!
    Pan the map by \a offset in pixels.

    The pixel coordinate origin is located at the upper left corner of the map.
*/
void QMapboxGL::moveBy(const QPointF &offset)
{
    d_ptr->mapObj->moveBy(mbgl::ScreenCoordinate { offset.x(), offset.y() });
}

/*!
    \fn QMapboxGL::scaleBy(double scale, const QPointF &center)

    Scale the map by \a scale in the direction of the \a center. This function
    can be used for implementing a pinch gesture.
*/
void QMapboxGL::scaleBy(double scale_, const QPointF &center) {
    d_ptr->mapObj->setZoom(d_ptr->mapObj->getZoom() + std::log2(scale_), mbgl::ScreenCoordinate { center.x(), center.y() });
}

/*!
    Rotate the map from the \a first screen coordinate to the \a second screen coordinate.
    This method can be used for implementing rotating the map by clicking and dragging,
    being \a first the cursor coordinate at the last frame and \a second the cursor coordinate
    at the current frame.
*/
void QMapboxGL::rotateBy(const QPointF &first, const QPointF &second)
{
    d_ptr->mapObj->rotateBy(
            mbgl::ScreenCoordinate { first.x(), first.y() },
            mbgl::ScreenCoordinate { second.x(), second.y() });
}

/*!
    Resize the map to \a size and scale to fit at \a framebufferSize. For
    high DPI screens, the size will be smaller than the \a framebufferSize.

    This fallowing example will double the pixel density of the map for
    a given \c size:

    \code
        map->resize(size / 2, size);
    \endcode
*/
void QMapboxGL::resize(const QSize& size, const QSize& framebufferSize)
{
    if (d_ptr->size == size && d_ptr->fbSize == framebufferSize) return;

    d_ptr->size = size;
    d_ptr->fbSize = framebufferSize;

    d_ptr->mapObj->setSize(sanitizedSize(size));
}

/*!
    If Mapbox GL needs to rebind the default framebuffer, it will use the
    ID supplied here.
*/
void QMapboxGL::setFramebufferObject(quint32 fbo) {
    d_ptr->fbObject = fbo;
}

/*!
    Adds an \a icon to the annotation icon pool. This can be later used by the annotation
    functions to shown any drawing on the map by referencing its \a name.

    Unlike using addIcon() for runtime styling, annotations added with addAnnotation()
    will survive style changes.

    \sa addAnnotation()
*/
void QMapboxGL::addAnnotationIcon(const QString &name, const QImage &icon)
{
    if (icon.isNull()) return;

    d_ptr->mapObj->addAnnotationImage(name.toStdString(), toStyleImage(icon));
}

/*!
    Returns the amount of meters per pixel from a given \a latitude and \a zoom.
*/
double QMapboxGL::metersPerPixelAtLatitude(double latitude, double zoom) const
{
    return mbgl::Projection::getMetersPerPixelAtLatitude(latitude, zoom);
}

/*!
    Return the projected meters for a given \a coordinate_ object.
*/
QMapbox::ProjectedMeters QMapboxGL::projectedMetersForCoordinate(const QMapbox::Coordinate &coordinate_) const
{
    auto projectedMeters = mbgl::Projection::projectedMetersForLatLng(mbgl::LatLng { coordinate_.first, coordinate_.second });
    return QMapbox::ProjectedMeters(projectedMeters.northing(), projectedMeters.easting());
}

/*!
    Returns the coordinate for a given \a projectedMeters object.
*/
QMapbox::Coordinate QMapboxGL::coordinateForProjectedMeters(const QMapbox::ProjectedMeters &projectedMeters) const
{
    auto latLng = mbgl::Projection::latLngForProjectedMeters(mbgl::ProjectedMeters { projectedMeters.first, projectedMeters.second });
    return QMapbox::Coordinate(latLng.latitude(), latLng.longitude());
}

/*!
    \fn QMapboxGL::pixelForCoordinate(const QMapbox::Coordinate &coordinate) const

    Returns the offset in pixels for \a coordinate. The origin pixel coordinate is
    located at the top left corner of the map view.

    This method returns the correct value for any coordinate, even if the coordinate
    is not currently visible on the screen.

    /note The return value is affected by the current zoom level, bearing and pitch.
*/
QPointF QMapboxGL::pixelForCoordinate(const QMapbox::Coordinate &coordinate_) const
{
    const mbgl::ScreenCoordinate pixel =
        d_ptr->mapObj->pixelForLatLng(mbgl::LatLng { coordinate_.first, coordinate_.second });

    return QPointF(pixel.x, pixel.y);
}

/*!
    Returns the geographic coordinate for the \a pixel coordinate.
*/
QMapbox::Coordinate QMapboxGL::coordinateForPixel(const QPointF &pixel) const
{
    const mbgl::LatLng latLng =
        d_ptr->mapObj->latLngForPixel(mbgl::ScreenCoordinate { pixel.x(), pixel.y() });

    return Coordinate(latLng.latitude(), latLng.longitude());
}

/*!
    Returns the coordinate and zoom combination needed in order to make the coordinate
    bounding box \a sw and \a ne visible.
*/
QMapbox::CoordinateZoom QMapboxGL::coordinateZoomForBounds(const QMapbox::Coordinate &sw, QMapbox::Coordinate &ne) const
{
    auto bounds = mbgl::LatLngBounds::hull(mbgl::LatLng { sw.first, sw.second }, mbgl::LatLng { ne.first, ne.second });
    mbgl::CameraOptions camera = d_ptr->mapObj->cameraForLatLngBounds(bounds, d_ptr->margins);

    return {{ (*camera.center).latitude(), (*camera.center).longitude() }, *camera.zoom };
}

/*!
    Returns the coordinate and zoom combination needed in order to make the coordinate
    bounding box \a sw and \a ne visible taking into account \a newBearing and \a newPitch.
*/
QMapbox::CoordinateZoom QMapboxGL::coordinateZoomForBounds(const QMapbox::Coordinate &sw, QMapbox::Coordinate &ne,
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

    return {{ (*camera.center).latitude(), (*camera.center).longitude() }, *camera.zoom };
}

/*!
    \property QMapboxGL::margins
    \brief the map margins in pixels from the corners of the map.

    This property sets a new reference center for the map.
*/
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
        d_ptr->margins.left(),
        d_ptr->margins.top(),
        d_ptr->margins.right(),
        d_ptr->margins.bottom()
    );
}

/*!
    Adds a source \a id to the map as specified by the \l
    {https://www.mapbox.com/mapbox-gl-style-spec/#root-sources}{Mapbox style specification} with
    \a params.

    This example reads a GeoJSON from the Qt resource system and adds it as source:

    \code
        QFile geojson(":source1.geojson");
        geojson.open(QIODevice::ReadOnly);

        QVariantMap routeSource;
        routeSource["type"] = "geojson";
        routeSource["data"] = geojson.readAll();

        map->addSource("routeSource", routeSource);
    \endcode
*/
void QMapboxGL::addSource(const QString &id, const QVariantMap &params)
{
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    Error error;
    mbgl::optional<std::unique_ptr<Source>> source = convert<std::unique_ptr<Source>>(QVariant(params), error, id.toStdString());
    if (!source) {
        qWarning() << "Unable to add source:" << error.message.c_str();
        return;
    }

    d_ptr->mapObj->addSource(std::move(*source));
}

/*!
    Returns true if the layer with given \a sourceID exists, false otherwise.
*/
bool QMapboxGL::sourceExists(const QString& sourceID)
{
    return !!d_ptr->mapObj->getSource(sourceID.toStdString());
}

/*!
    Updates the source \a id with new \a params.

    If the source does not exist, it will be added like in addSource(). Only
    GeoJSON sources can be updated.
*/
void QMapboxGL::updateSource(const QString &id, const QVariantMap &params)
{
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    auto source = d_ptr->mapObj->getSource(id.toStdString());
    if (!source) {
        addSource(id, params);
        return;
    }

    auto sourceGeoJSON = source->as<GeoJSONSource>();
    if (!sourceGeoJSON) {
        qWarning() << "Unable to update source: only GeoJSON sources are mutable.";
        return;
    }

    if (params.contains("data")) {
        Error error;
        auto result = convert<mbgl::GeoJSON>(params["data"], error);
        if (result) {
            sourceGeoJSON->setGeoJSON(*result);
        }
    }
}

/*!
    Removes the source \a id.

    This method has no effect if the source does not exist.
*/
void QMapboxGL::removeSource(const QString& id)
{
    auto sourceIDStdString = id.toStdString();

    if (d_ptr->mapObj->getSource(sourceIDStdString)) {
        d_ptr->mapObj->removeSource(sourceIDStdString);
    }
}

/*!
    Adds a custom layer \a id with the initialization function \a initFn, the
    render function \a renderFn and the deinitialization function \a deinitFn with
    the user data \a context before the existing layer \a before.

    \warning This is used for delegating the rendering of a layer to the user of
    this API and is not officially supported. Use at your own risk.
*/
void QMapboxGL::addCustomLayer(const QString &id,
        QMapbox::CustomLayerInitializeFunction initFn,
        QMapbox::CustomLayerRenderFunction renderFn,
        QMapbox::CustomLayerDeinitializeFunction deinitFn,
        void *context,
        char *before)
{
    d_ptr->mapObj->addLayer(std::make_unique<mbgl::style::CustomLayer>(
            id.toStdString(),
            reinterpret_cast<mbgl::style::CustomLayerInitializeFunction>(initFn),
            // This cast is safe as long as both mbgl:: and QMapbox::
            // CustomLayerRenderParameters members remains the same.
            (mbgl::style::CustomLayerRenderFunction)renderFn,
            reinterpret_cast<mbgl::style::CustomLayerDeinitializeFunction>(deinitFn),
            context),
            before ? mbgl::optional<std::string>(before) : mbgl::optional<std::string>());
}

/*!
    Adds a style layer to the map as specified by the \l
    {https://www.mapbox.com/mapbox-gl-style-spec/#root-layers}{Mapbox style specification} with
    \a params.

    This example shows how to add a layer that will be used to show a route line on the map. Note
    that nothing will be drawn until we set paint properties using setPaintProperty().

    \code
        QVariantMap route;
        route["id"] = "route";
        route["type"] = "line";
        route["source"] = "routeSource";

        map->addLayer(route);
    \endcode

    /note The source must exist prior to adding a layer.
*/
void QMapboxGL::addLayer(const QVariantMap &params)
{
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    Error error;
    mbgl::optional<std::unique_ptr<Layer>> layer = convert<std::unique_ptr<Layer>>(QVariant(params), error);
    if (!layer) {
        qWarning() << "Unable to add layer:" << error.message.c_str();
        return;
    }

    d_ptr->mapObj->addLayer(std::move(*layer));
}

/*!
    Returns true if the layer with given \a id exists, false otherwise.
*/
bool QMapboxGL::layerExists(const QString& id)
{
    return !!d_ptr->mapObj->getLayer(id.toStdString());
}

/*!
    Removes the layer with given \a id.
*/
void QMapboxGL::removeLayer(const QString& id)
{
    d_ptr->mapObj->removeLayer(id.toStdString());
}

/*!
    Adds the \a image with the identifier \a id that can be used
    later by a symbol layer.

    If the \a id was already added, it gets replaced by the new
    \a image only if the dimensions of the image are the same as
    the old image, otherwise it has no effect.

    \sa addLayer()
*/
void QMapboxGL::addImage(const QString &id, const QImage &image)
{
    if (image.isNull()) return;

    d_ptr->mapObj->addImage(id.toStdString(), toStyleImage(image));
}

/*!
    Removes the image \a id.
*/
void QMapboxGL::removeImage(const QString &id)
{
    d_ptr->mapObj->removeImage(id.toStdString());
}

/*!
    Adds a \a filter to a style \a layer using the format described in the \l
    {https://www.mapbox.com/mapbox-gl-style-spec/#types-filter}{Mapbox style specification}.

    Given a layer \c marker from an arbitrary GeoJSON source containing features of type \b
    "Point" and \b "LineString", this example shows how to make sure the layer will only tag
    features of type \b "Point".

    \code
        QVariantList filterExpression;
        filterExpression.append("==");
        filterExpression.append("$type");
        filterExpression.append("Point");

        QVariantList filter;
        filter.append(filterExpression);

        map->setFilter("marker", filter);
    \endcode
*/
void QMapboxGL::setFilter(const QString& layer, const QVariant& filter)
{
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    Layer* layer_ = d_ptr->mapObj->getLayer(layer.toStdString());
    if (!layer_) {
        qWarning() << "Layer not found:" << layer;
        return;
    }

    Filter filter_;

    Error error;
    mbgl::optional<Filter> converted = convert<Filter>(filter, error);
    if (!converted) {
        qWarning() << "Error parsing filter:" << error.message.c_str();
        return;
    }
    filter_ = std::move(*converted);

    if (layer_->is<FillLayer>()) {
        layer_->as<FillLayer>()->setFilter(filter_);
        return;
    }
    if (layer_->is<LineLayer>()) {
        layer_->as<LineLayer>()->setFilter(filter_);
        return;
    }
    if (layer_->is<SymbolLayer>()) {
        layer_->as<SymbolLayer>()->setFilter(filter_);
        return;
    }
    if (layer_->is<CircleLayer>()) {
        layer_->as<CircleLayer>()->setFilter(filter_);
        return;
    }
    if (layer_->is<FillExtrusionLayer>()) {
        layer_->as<FillExtrusionLayer>()->setFilter(filter_);
        return;
    }

    qWarning() << "Layer doesn't support filters";
}

/*!
    Renders the map using OpenGL draw calls. If \a fbo is passed, it will
    make sure to bind the framebuffer object before drawing; otherwise a
    valid OpenGL context is expected with an appropriate OpenGL viewport state set
    for the size of the canvas.

    This function should be called only after the signal needsRendering() is
    emitted at least once.
*/
void QMapboxGL::render()
{
#if defined(__APPLE__) && QT_VERSION < 0x050000
    // FIXME Qt 4.x provides an incomplete FBO at start.
    // See https://bugreports.qt.io/browse/QTBUG-36802 for details.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return;
    }
#endif

    // The OpenGL implementation automatically enables the OpenGL context for us.
    mbgl::BackendScope scope { *d_ptr, mbgl::BackendScope::ScopeType::Implicit };

    d_ptr->dirty = false;
    d_ptr->mapObj->render(*d_ptr);
}

/*!
    Informs the map that the network connection has been established, causing
    all network requests that previously timed out to be retried immediately.
*/
void QMapboxGL::connectionEstablished()
{
    d_ptr->connectionEstablished();
}

/*!
    \fn void QMapboxGL::needsRendering()

    This signal is emitted when the visual contents of the map have changed
    and a redraw is needed in order to keep the map visually consistent
    with the current state.

    \sa render()
*/

/*!
    \fn void QMapboxGL::mapChanged(QMapboxGL::MapChange change)

    This signal is emitted when the state of the map has changed. This signal
    may be used for detecting errors when loading a style or detecting when
    a map is fully loaded by analyzing the parameter \a change.
*/

/*!
    \fn void QMapboxGL::copyrightsChanged(const QString &copyrightsHtml);

    This signal is emitted when the copyrights of the current content of the map
    have changed. This can be caused by a style change or adding a new source.

    \a copyrightsHtml is a string with a HTML snippet.
*/

QMapboxGLPrivate::QMapboxGLPrivate(QMapboxGL *q, const QMapboxGLSettings &settings, const QSize &size_, qreal pixelRatio)
    : QObject(q)
    , size(size_)
    , q_ptr(q)
    , fileSourceObj(std::make_unique<mbgl::DefaultFileSource>(
        settings.cacheDatabasePath().toStdString(),
        settings.assetPath().toStdString(),
        settings.cacheDatabaseMaximumSize()))
    , threadPool(mbgl::sharedThreadPool())
{
    mapObj = std::make_unique<mbgl::Map>(
            *this, sanitizedSize(size),
            pixelRatio, *fileSourceObj, *threadPool,
            mbgl::MapMode::Continuous,
            static_cast<mbgl::GLContextMode>(settings.contextMode()),
            static_cast<mbgl::ConstrainMode>(settings.constrainMode()),
            static_cast<mbgl::ViewportMode>(settings.viewportMode()));

    qRegisterMetaType<QMapboxGL::MapChange>("QMapboxGL::MapChange");

    fileSourceObj->setAccessToken(settings.accessToken().toStdString());
    fileSourceObj->setAPIBaseURL(settings.apiBaseUrl().toStdString());

    connect(this, SIGNAL(needsRendering()), q_ptr, SIGNAL(needsRendering()), Qt::QueuedConnection);
    connect(this, SIGNAL(mapChanged(QMapboxGL::MapChange)), q_ptr, SIGNAL(mapChanged(QMapboxGL::MapChange)), Qt::QueuedConnection);
    connect(this, SIGNAL(copyrightsChanged(QString)), q_ptr, SIGNAL(copyrightsChanged(QString)), Qt::QueuedConnection);
}

QMapboxGLPrivate::~QMapboxGLPrivate()
{
}

mbgl::Size QMapboxGLPrivate::framebufferSize() const {
    return sanitizedSize(fbSize);
}

void QMapboxGLPrivate::updateAssumedState() {
    assumeFramebufferBinding(fbObject);
    assumeViewportSize(framebufferSize());
}

void QMapboxGLPrivate::bind() {
    setFramebufferBinding(fbObject);
    setViewportSize(framebufferSize());
}

void QMapboxGLPrivate::invalidate()
{
    if (!dirty) {
        emit needsRendering();
        dirty = true;
    }
}

void QMapboxGLPrivate::onCameraWillChange(mbgl::MapObserver::CameraChangeMode mode)
{
    if (mode == mbgl::MapObserver::CameraChangeMode::Immediate) {
        emit mapChanged(QMapboxGL::MapChangeRegionWillChange);
    } else {
        emit mapChanged(QMapboxGL::MapChangeRegionWillChangeAnimated);
    }
}

void QMapboxGLPrivate::onCameraIsChanging()
{
    emit mapChanged(QMapboxGL::MapChangeRegionIsChanging);
}

void QMapboxGLPrivate::onCameraDidChange(mbgl::MapObserver::CameraChangeMode mode)
{
    if (mode == mbgl::MapObserver::CameraChangeMode::Immediate) {
        emit mapChanged(QMapboxGL::MapChangeRegionDidChange);
    } else {
        emit mapChanged(QMapboxGL::MapChangeRegionDidChangeAnimated);
    }
}

void QMapboxGLPrivate::onWillStartLoadingMap()
{
    emit mapChanged(QMapboxGL::MapChangeWillStartLoadingMap);
}

void QMapboxGLPrivate::onDidFinishLoadingMap()
{
    emit mapChanged(QMapboxGL::MapChangeDidFinishLoadingMap);
}

void QMapboxGLPrivate::onDidFailLoadingMap(std::exception_ptr)
{
    emit mapChanged(QMapboxGL::MapChangeDidFailLoadingMap);
}

void QMapboxGLPrivate::onWillStartRenderingFrame()
{
    emit mapChanged(QMapboxGL::MapChangeWillStartRenderingFrame);
}

void QMapboxGLPrivate::onDidFinishRenderingFrame(mbgl::MapObserver::RenderMode mode)
{
    if (mode == mbgl::MapObserver::RenderMode::Partial) {
        emit mapChanged(QMapboxGL::MapChangeDidFinishRenderingFrame);
    } else {
        emit mapChanged(QMapboxGL::MapChangeDidFinishRenderingFrameFullyRendered);
    }
}

void QMapboxGLPrivate::onWillStartRenderingMap()
{
    emit mapChanged(QMapboxGL::MapChangeWillStartLoadingMap);
}

void QMapboxGLPrivate::onDidFinishRenderingMap(mbgl::MapObserver::RenderMode mode)
{
    if (mode == mbgl::MapObserver::RenderMode::Partial) {
        emit mapChanged(QMapboxGL::MapChangeDidFinishRenderingMap);
    } else {
        emit mapChanged(QMapboxGL::MapChangeDidFinishRenderingMapFullyRendered);
    }
}

void QMapboxGLPrivate::onDidFinishLoadingStyle()
{
    emit mapChanged(QMapboxGL::MapChangeDidFinishLoadingStyle);
}

void QMapboxGLPrivate::onSourceChanged(mbgl::style::Source&)
{
    std::string attribution;
    for (const auto& source : mapObj->getSources()) {
        // Avoid duplicates by using the most complete attribution HTML snippet.
        if (source->getAttribution() && (attribution.size() < source->getAttribution()->size()))
            attribution = *source->getAttribution();
    }
    emit copyrightsChanged(QString::fromStdString(attribution));
    emit mapChanged(QMapboxGL::MapChangeSourceDidChange);
}

/*!
    Initializes an OpenGL extension function such as Vertex Array Objects (VAOs),
    required by Mapbox GL Native engine.
*/
mbgl::gl::ProcAddress QMapboxGLPrivate::initializeExtension(const char* name) {
#if QT_VERSION >= 0x050000
    QOpenGLContext* thisContext = QOpenGLContext::currentContext();
    return thisContext->getProcAddress(name);
#else
    const QGLContext* thisContext = QGLContext::currentContext();
    return reinterpret_cast<mbgl::gl::ProcAddress>(thisContext->getProcAddress(name));
#endif
}

void QMapboxGLPrivate::connectionEstablished()
{
    mbgl::NetworkStatus::Reachable();
}
