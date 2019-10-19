#include "qmapboxgl.hpp"
#include "qmapboxgl_p.hpp"

#include "qmapboxgl_map_observer.hpp"
#include "qmapboxgl_renderer_observer.hpp"

#include "qt_conversion.hpp"
#include "qt_geojson.hpp"

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/shared_thread_pool.hpp>
#include <mbgl/util/traits.hpp>
#include <mbgl/actor/scheduler.hpp>

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#else
#include <QCoreApplication>
#endif

#include <QDebug>
#include <QImage>
#include <QMargins>
#include <QString>
#include <QStringList>
#include <QThreadStorage>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QColor>

#include <memory>

using namespace QMapbox;

// mbgl::GLContextMode
static_assert(mbgl::underlying_type(QMapboxGLSettings::UniqueGLContext) == mbgl::underlying_type(mbgl::GLContextMode::Unique), "error");
static_assert(mbgl::underlying_type(QMapboxGLSettings::SharedGLContext) == mbgl::underlying_type(mbgl::GLContextMode::Shared), "error");

// mbgl::MapMode
static_assert(mbgl::underlying_type(QMapboxGLSettings::Continuous) == mbgl::underlying_type(mbgl::MapMode::Continuous), "error");
static_assert(mbgl::underlying_type(QMapboxGLSettings::Static) == mbgl::underlying_type(mbgl::MapMode::Static), "error");

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

std::shared_ptr<mbgl::DefaultFileSource> sharedDefaultFileSource(
        const std::string& cachePath, const std::string& assetRoot, uint64_t maximumCacheSize) {
    static std::mutex mutex;
    static std::unordered_map<std::string, std::weak_ptr<mbgl::DefaultFileSource>> fileSources;

    std::lock_guard<std::mutex> lock(mutex);

    // Purge entries no longer in use.
    for (auto it = fileSources.begin(); it != fileSources.end();) {
        if (!it->second.lock()) {
            it = fileSources.erase(it);
        } else {
            ++it;
        }
    }

    // Return an existing FileSource if available.
    auto sharedFileSource = fileSources.find(cachePath);
    if (sharedFileSource != fileSources.end()) {
        return sharedFileSource->second.lock();
    }

    // New path, create a new FileSource.
    auto newFileSource = std::make_shared<mbgl::DefaultFileSource>(
        cachePath, assetRoot, maximumCacheSize);

    fileSources[cachePath] = newFileSource;

    return newFileSource;
}

// Conversion helper functions.

mbgl::Size sanitizedSize(const QSize& size) {
    return mbgl::Size {
        mbgl::util::max(0u, static_cast<uint32_t>(size.width())),
        mbgl::util::max(0u, static_cast<uint32_t>(size.height())),
    };
};

std::unique_ptr<mbgl::style::Image> toStyleImage(const QString &id, const QImage &sprite) {
    const QImage swapped = sprite
        .rgbSwapped()
        .convertToFormat(QImage::Format_ARGB32_Premultiplied);

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    auto img = std::make_unique<uint8_t[]>(swapped.sizeInBytes());
    memcpy(img.get(), swapped.constBits(), swapped.sizeInBytes());
#else
    auto img = std::make_unique<uint8_t[]>(swapped.byteCount());
    memcpy(img.get(), swapped.constBits(), swapped.byteCount());
#endif

    return std::make_unique<mbgl::style::Image>(
        id.toStdString(),
        mbgl::PremultipliedImage(
            { static_cast<uint32_t>(swapped.width()), static_cast<uint32_t>(swapped.height()) },
            std::move(img)),
        1.0);
}

} // namespace

/*!
    \class QMapboxGLSettings
    \brief The QMapboxGLSettings class stores the initial configuration for QMapboxGL.

    \inmodule Mapbox Maps SDK for Qt

    QMapboxGLSettings is used to configure QMapboxGL at the moment of its creation.
    Once created, the QMapboxGLSettings of a QMapboxGL can no longer be changed.

    Cache-related settings are shared between all QMapboxGL instances using the same cache path.
    The first map to configure cache properties such as size will force the configuration
    to all newly instantiated QMapboxGL objects using the same cache in the same process.

    \since 4.7
*/

/*!
    \enum QMapboxGLSettings::GLContextMode

    This enum sets the expectations for the OpenGL state.

    \value UniqueGLContext  The OpenGL context is only used by QMapboxGL, so it is not
    reset before each rendering. Use this mode if the intention is to only draw a
    fullscreen map.

    \value SharedGLContext  The OpenGL context is shared and the state will be
    marked dirty - which invalidates any previously assumed GL state. The
    embedder is responsible for clearing up the viewport prior to calling
    QMapboxGL::render. The embedder is also responsible for resetting its own
    GL state after QMapboxGL::render has finished, if needed.

    \sa contextMode()
*/

/*!
    \enum QMapboxGLSettings::MapMode

    This enum sets the map rendering mode

    \value Continuous  The map will render as data arrives from the network and
    react immediately to state changes.

    This is the default mode and the preferred when the map is intended to be
    interactive.

    \value Static  The map will no longer react to state changes and will only
    be rendered when QMapboxGL::startStaticRender is called. After all the
    resources are loaded, the QMapboxGL::staticRenderFinished signal is emitted.

    This mode is useful for taking a snapshot of the finished rendering result
    of the map into a QImage.

    \sa mapMode()
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
    , m_mapMode(QMapboxGLSettings::Continuous)
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
    Returns the map mode. Static mode will emit a signal for
    rendering a map only when the map is fully loaded.
    Animations like style transitions and labels fading won't
    be seen.

    The Continuous mode will emit the signal for every new
    change on the map and it is usually what you expect for
    a interactive map.

    By default, it is set to QMapboxGLSettings::Continuous.
*/
QMapboxGLSettings::MapMode QMapboxGLSettings::mapMode() const
{
    return m_mapMode;
}

/*!
    Sets the map \a mode.
*/
void QMapboxGLSettings::setMapMode(MapMode mode)
{
    m_mapMode = mode;
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
    Returns resource transformation callback used to transform requested URLs.
*/
std::function<std::string(const std::string &&)> QMapboxGLSettings::resourceTransform() const
{
    return m_resourceTransform;
}

/*!
    Sets the resource \a transform callback.

    When given, resource transformation callback will be used to transform the
    requested resource URLs before they are requested from internet. This can be
    used add or remove custom parameters, or reroute certain requests to other
    servers or endpoints.
*/
void QMapboxGLSettings::setResourceTransform(const std::function<std::string(const std::string &&)> &transform)
{
    m_resourceTransform = transform;
}

/*!
    \class QMapboxGL
    \brief The QMapboxGL class is a Qt wrapper for the Mapbox GL Native engine.

    \inmodule Mapbox Maps SDK for Qt

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
    \enum QMapboxGL::MapLoadingFailure

    This enum represents map loading failure type.

    \value StyleParseFailure                             Failure to parse the style.
    \value StyleLoadFailure                              Failure to load the style data.
    \value NotFoundFailure                               Failure to obtain style resource file.
    \value UnknownFailure                                Unknown map loading failure.

    \sa mapLoadingFailed()
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
    Constructs a QMapboxGL object with \a settings and sets \a parent_ as the parent
    object. The \a settings cannot be changed after the object is constructed. The
    \a size represents the size of the viewport and the \a pixelRatio the initial pixel
    density of the screen.
*/
QMapboxGL::QMapboxGL(QObject *parent_, const QMapboxGLSettings &settings, const QSize& size, qreal pixelRatio)
    : QObject(parent_)
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
    return QString::fromStdString(d_ptr->mapObj->getStyle().getJSON());
}

void QMapboxGL::setStyleJson(const QString &style)
{
    d_ptr->mapObj->getStyle().loadJSON(style.toStdString());
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
    return QString::fromStdString(d_ptr->mapObj->getStyle().getURL());
}

void QMapboxGL::setStyleUrl(const QString &url)
{
    d_ptr->mapObj->getStyle().loadURL(url.toStdString());
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
    d_ptr->mapObj->setZoom(::log2(scale_), mbgl::ScreenCoordinate { center.x(), center.y() });
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
    Sets the \a duration and \a delay of style transitions. Style paint property
    values transition to new values with animation when they are updated.
*/
void QMapboxGL::setTransitionOptions(qint64 duration, qint64 delay) {
    static auto convert = [](qint64 value) -> mbgl::optional<mbgl::Duration> {
        return std::chrono::duration_cast<mbgl::Duration>(mbgl::Milliseconds(value));
    };

    d_ptr->mapObj->getStyle().setTransitionOptions(mbgl::style::TransitionOptions{ convert(duration), convert(delay) });
}

mbgl::optional<mbgl::Annotation> asMapboxGLAnnotation(const QMapbox::Annotation & annotation) {
    auto asMapboxGLGeometry = [](const QMapbox::ShapeAnnotationGeometry &geometry) {
        mbgl::ShapeAnnotationGeometry result;
        switch (geometry.type) {
        case QMapbox::ShapeAnnotationGeometry::LineStringType:
            result = asMapboxGLLineString(geometry.geometry.first().first());
            break;
        case QMapbox::ShapeAnnotationGeometry::PolygonType:
            result = asMapboxGLPolygon(geometry.geometry.first());
            break;
        case QMapbox::ShapeAnnotationGeometry::MultiLineStringType:
            result = asMapboxGLMultiLineString(geometry.geometry.first());
            break;
        case QMapbox::ShapeAnnotationGeometry::MultiPolygonType:
            result = asMapboxGLMultiPolygon(geometry.geometry);
            break;
        }
        return result;
    };

    if (annotation.canConvert<QMapbox::SymbolAnnotation>()) {
        QMapbox::SymbolAnnotation symbolAnnotation = annotation.value<QMapbox::SymbolAnnotation>();
        QMapbox::Coordinate& pair = symbolAnnotation.geometry;
        return { mbgl::SymbolAnnotation(mbgl::Point<double> { pair.second, pair.first }, symbolAnnotation.icon.toStdString()) };
    } else if (annotation.canConvert<QMapbox::LineAnnotation>()) {
        QMapbox::LineAnnotation lineAnnotation = annotation.value<QMapbox::LineAnnotation>();
        auto color = mbgl::Color::parse(lineAnnotation.color.name().toStdString());
        return { mbgl::LineAnnotation(asMapboxGLGeometry(lineAnnotation.geometry), lineAnnotation.opacity, lineAnnotation.width, { *color }) };
    } else if (annotation.canConvert<QMapbox::FillAnnotation>()) {
        QMapbox::FillAnnotation fillAnnotation = annotation.value<QMapbox::FillAnnotation>();
        auto color = mbgl::Color::parse(fillAnnotation.color.name().toStdString());
        if (fillAnnotation.outlineColor.canConvert<QColor>()) {
            auto outlineColor = mbgl::Color::parse(fillAnnotation.outlineColor.value<QColor>().name().toStdString());
            return { mbgl::FillAnnotation(asMapboxGLGeometry(fillAnnotation.geometry), fillAnnotation.opacity, { *color }, { *outlineColor }) };
        } else {
            return { mbgl::FillAnnotation(asMapboxGLGeometry(fillAnnotation.geometry), fillAnnotation.opacity, { *color }, {}) };
        }
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
    return d_ptr->mapObj->addAnnotation(*asMapboxGLAnnotation(annotation));
}

/*!
    Updates an existing \a annotation referred by \a id.

    \sa addAnnotationIcon()
*/
void QMapboxGL::updateAnnotation(QMapbox::AnnotationID id, const QMapbox::Annotation &annotation)
{
    d_ptr->mapObj->updateAnnotation(id, *asMapboxGLAnnotation(annotation));
}

/*!
    Removes an existing annotation referred by \a id.
*/
void QMapboxGL::removeAnnotation(QMapbox::AnnotationID id)
{
    d_ptr->mapObj->removeAnnotation(id);
}

/*!
    Sets a layout \a property_ \a value to an existing \a layer. The \a property_ string can be any
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
void QMapboxGL::setLayoutProperty(const QString& layer, const QString& property_, const QVariant& value)
{
    using namespace mbgl::style;

    Layer* layer_ = d_ptr->mapObj->getStyle().getLayer(layer.toStdString());
    if (!layer_) {
        qWarning() << "Layer not found:" << layer;
        return;
    }

    if (conversion::setLayoutProperty(*layer_, property_.toStdString(), value)) {
        qWarning() << "Error setting layout property:" << layer << "-" << property_;
        return;
    }
}

/*!
    Sets a paint \a property_ \a value to an existing \a layer. The \a property_ string can be any
    as defined by the \l {https://www.mapbox.com/mapbox-gl-style-spec/} {Mapbox style specification}
    for paint properties.

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
void QMapboxGL::setPaintProperty(const QString& layer, const QString& property_, const QVariant& value)
{
    using namespace mbgl::style;

    Layer* layer_ = d_ptr->mapObj->getStyle().getLayer(layer.toStdString());
    if (!layer_) {
        qWarning() << "Layer not found:" << layer;
        return;
    }

    if (conversion::setPaintProperty(*layer_, property_.toStdString(), value)) {
        qWarning() << "Error setting paint property:" << layer << "-" << property_;
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
    d_ptr->mapObj->setZoom(d_ptr->mapObj->getZoom() + ::log2(scale_), mbgl::ScreenCoordinate { center.x(), center.y() });
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
    Resize the map to \a size_ and scale to fit at the framebuffer. For
    high DPI screens, the size will be smaller than the framebuffer.
*/
void QMapboxGL::resize(const QSize& size_)
{
    auto size = sanitizedSize(size_);

    if (d_ptr->mapObj->getSize() == size)
        return;

    d_ptr->mapObj->setSize(size);
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

    d_ptr->mapObj->addAnnotationImage(toStyleImage(name, icon));
}

/*!
    Returns the amount of meters per pixel from a given \a latitude_ and \a zoom_.
*/
double QMapboxGL::metersPerPixelAtLatitude(double latitude_, double zoom_) const
{
    return mbgl::Projection::getMetersPerPixelAtLatitude(latitude_, zoom_);
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

    d_ptr->mapObj->getStyle().addSource(std::move(*source));
}

/*!
    Returns true if the layer with given \a sourceID exists, false otherwise.
*/
bool QMapboxGL::sourceExists(const QString& sourceID)
{
    return !!d_ptr->mapObj->getStyle().getSource(sourceID.toStdString());
}

/*!
    Updates the source \a id with new \a params.

    If the source does not exist, it will be added like in addSource(). Only
    image and GeoJSON sources can be updated.
*/
void QMapboxGL::updateSource(const QString &id, const QVariantMap &params)
{
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    auto source = d_ptr->mapObj->getStyle().getSource(id.toStdString());
    if (!source) {
        addSource(id, params);
        return;
    }

    auto sourceGeoJSON = source->as<GeoJSONSource>();
    auto sourceImage = source->as<ImageSource>();
    if (!sourceGeoJSON && !sourceImage) {
        qWarning() << "Unable to update source: only GeoJSON and Image sources are mutable.";
        return;
    }

    if (sourceImage) {
        if (params.contains("url")) {
            sourceImage->setURL(params["url"].toString().toStdString());
        }
    } else if (sourceGeoJSON && params.contains("data")) {
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

    if (d_ptr->mapObj->getStyle().getSource(sourceIDStdString)) {
        d_ptr->mapObj->getStyle().removeSource(sourceIDStdString);
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
        QScopedPointer<QMapbox::CustomLayerHostInterface>& host,
        const QString& before)
{
    class HostWrapper : public mbgl::style::CustomLayerHost {
        public:
        QScopedPointer<QMapbox::CustomLayerHostInterface> ptr;
        HostWrapper(QScopedPointer<QMapbox::CustomLayerHostInterface>& p)
         : ptr(p.take()) {
         }

        void initialize() {
            ptr->initialize(); 
        }

        void render(const mbgl::style::CustomLayerRenderParameters& params) {
            QMapbox::CustomLayerRenderParameters renderParams;
            renderParams.width = params.width;
            renderParams.height = params.height;
            renderParams.latitude = params.latitude;
            renderParams.longitude = params.longitude;
            renderParams.zoom = params.zoom;
            renderParams.bearing = params.bearing;
            renderParams.pitch = params.pitch;
            renderParams.fieldOfView = params.fieldOfView;
            ptr->render(renderParams);
        }

        void contextLost() { }

        void deinitialize() {
            ptr->deinitialize();
        }
    };

    d_ptr->mapObj->getStyle().addLayer(std::make_unique<mbgl::style::CustomLayer>(
            id.toStdString(),
            std::make_unique<HostWrapper>(host)),
            before.isEmpty() ? mbgl::optional<std::string>() : mbgl::optional<std::string>(before.toStdString()));
}

/*!
    Adds a style layer to the map as specified by the \l
    {https://www.mapbox.com/mapbox-gl-style-spec/#root-layers}{Mapbox style specification} with
    \a params. The layer will be added under the layer specified by \a before, if specified.
    Otherwise it will be added as the topmost layer.

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
void QMapboxGL::addLayer(const QVariantMap &params, const QString& before)
{
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    Error error;
    mbgl::optional<std::unique_ptr<Layer>> layer = convert<std::unique_ptr<Layer>>(QVariant(params), error);
    if (!layer) {
        qWarning() << "Unable to add layer:" << error.message.c_str();
        return;
    }

    d_ptr->mapObj->getStyle().addLayer(std::move(*layer),
        before.isEmpty() ? mbgl::optional<std::string>() : mbgl::optional<std::string>(before.toStdString()));
}

/*!
    Returns true if the layer with given \a id exists, false otherwise.
*/
bool QMapboxGL::layerExists(const QString& id)
{
    return !!d_ptr->mapObj->getStyle().getLayer(id.toStdString());
}

/*!
    Removes the layer with given \a id.
*/
void QMapboxGL::removeLayer(const QString& id)
{
    d_ptr->mapObj->getStyle().removeLayer(id.toStdString());
}

/*!
    List of all existing layer ids from the current style.
*/
QList<QString> QMapboxGL::layerIds() const
{
    const auto &layers = d_ptr->mapObj->getStyle().getLayers();

    QList<QString> layerIds;
    layerIds.reserve(layers.size());

    for (const mbgl::style::Layer *layer : layers) {
        layerIds.append(QString::fromStdString(layer->getID()));
    }

    return layerIds;
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

    d_ptr->mapObj->getStyle().addImage(toStyleImage(id, image));
}

/*!
    Removes the image \a id.
*/
void QMapboxGL::removeImage(const QString &id)
{
    d_ptr->mapObj->getStyle().removeImage(id.toStdString());
}

/*!
    Adds a \a filter to a style \a layer using the format described in the \l
    {https://www.mapbox.com/mapbox-gl-js/style-spec/#other-filter}{Mapbox style specification}.

    Given a layer \c marker from an arbitrary GeoJSON source containing features of type \b
    "Point" and \b "LineString", this example shows how to make sure the layer will only tag
    features of type \b "Point".

    \code
        QVariantList filterExpression;
        filterExpression.push_back(QLatin1String("=="));
        filterExpression.push_back(QLatin1String("$type"));
        filterExpression.push_back(QLatin1String("Point"));

        QVariantList filter;
        filter.push_back(filterExpression);

        map->setFilter(QLatin1String("marker"), filter);
    \endcode
*/
void QMapboxGL::setFilter(const QString& layer, const QVariant& filter)
{
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    Layer* layer_ = d_ptr->mapObj->getStyle().getLayer(layer.toStdString());
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

QVariant QVariantFromValue(const mbgl::Value &value) {
    return value.match(
        [](const mbgl::NullValue) {
            return QVariant();
        }, [](const bool value_) {
            return QVariant(value_);
        }, [](const float value_) {
            return QVariant(value_);
        }, [](const int64_t value_) {
            return QVariant(static_cast<qlonglong>(value_));
        }, [](const double value_) {
            return QVariant(value_);
        }, [](const std::string &value_) {
           return QVariant(value_.c_str());
        }, [](const mbgl::Color &value_) {
            return QColor(value_.r, value_.g, value_.b, value_.a);
        }, [&](const std::vector<mbgl::Value> &vector) {
            QVariantList list;
            list.reserve(vector.size());
            for (const auto &value_ : vector) {
                list.push_back(QVariantFromValue(value_));
            }
            return list;
        }, [&](const std::unordered_map<std::string, mbgl::Value> &map) {
            QVariantMap varMap;
            for (auto &item : map) {
                varMap.insert(item.first.c_str(), QVariantFromValue(item.second));
            }
            return varMap;
        }, [](const auto &) {
            return QVariant();
        });
}

/*!
    Returns the current \a expression-based filter value applied to a style
    \layer, if any.

    Filter value types are described in the {https://www.mapbox.com/mapbox-gl-js/style-spec/#types}{Mapbox style specification}.
*/
QVariant QMapboxGL::getFilter(const QString &layer)  const {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    Layer* layer_ = d_ptr->mapObj->getStyle().getLayer(layer.toStdString());
    if (!layer_) {
        qWarning() << "Layer not found:" << layer;
        return QVariant();
    }

    Filter filter_;

    if (layer_->is<FillLayer>()) {
        filter_ = layer_->as<FillLayer>()->getFilter();
    } else if (layer_->is<LineLayer>()) {
        filter_ = layer_->as<LineLayer>()->getFilter();
    } else if (layer_->is<SymbolLayer>()) {
        filter_ = layer_->as<SymbolLayer>()->getFilter();
    } else if (layer_->is<CircleLayer>()) {
        filter_ = layer_->as<CircleLayer>()->getFilter();
    } else if (layer_->is<FillExtrusionLayer>()) {
        filter_ = layer_->as<FillExtrusionLayer>()->getFilter();
    } else {
        qWarning() << "Layer doesn't support filters";
        return QVariant();
    }

    auto serialized = filter_.serialize();
    return QVariantFromValue(serialized);
}

/*!
    Creates the infrastructure needed for rendering the map. It
    should be called before any call to render().

    Must be called on the render thread.
*/
void QMapboxGL::createRenderer()
{
    d_ptr->createRenderer();
}

/*!
    Destroys the infrastructure needed for rendering the map,
    releasing resources.

    Must be called on the render thread.
*/
void QMapboxGL::destroyRenderer()
{
    d_ptr->destroyRenderer();
}

/*!
    Start a static rendering of the current state of the map. This
    should only be called when the map is initialized in static mode.

    \sa QMapboxGLSettings::MapMode
*/
void QMapboxGL::startStaticRender()
{
    d_ptr->mapObj->renderStill([this](std::exception_ptr err) {
        QString what;

        try {
            if (err) {
                std::rethrow_exception(err);
            }
        } catch(const std::exception& e) {
            what = e.what();
        }

        emit staticRenderFinished(what);
    });
}

/*!
    Renders the map using OpenGL draw calls. It will make sure to bind the
    framebuffer object before drawing; otherwise a valid OpenGL context is
    expected with an appropriate OpenGL viewport state set for the size of
    the canvas.

    This function should be called only after the signal needsRendering() is
    emitted at least once.

    Must be called on the render thread.
*/
void QMapboxGL::render()
{
    d_ptr->render();
}

/*!
    If Mapbox GL needs to rebind the default \a fbo, it will use the
    ID supplied here. \a size is the size of the framebuffer, which
    on high DPI screens is usually bigger than the map size.

    Must be called on the render thread.
*/
void QMapboxGL::setFramebufferObject(quint32 fbo, const QSize& size)
{
    d_ptr->setFramebufferObject(fbo, size);
}

/*!
    Informs the map that the network connection has been established, causing
    all network requests that previously timed out to be retried immediately.
*/
void QMapboxGL::connectionEstablished()
{
    mbgl::NetworkStatus::Reachable();
}

/*!
    \fn void QMapboxGL::needsRendering()

    This signal is emitted when the visual contents of the map have changed
    and a redraw is needed in order to keep the map visually consistent
    with the current state.

    \sa render()
*/

/*!
    \fn void QMapboxGL::staticRenderFinished(const QString &error)

    This signal is emitted when a static map is fully drawn. Usually the next
    step is to extract the map from a framebuffer into a container like a
    QImage. \a error is set to a message when an error occurs.

    \sa startStaticRender()
*/

/*!
    \fn void QMapboxGL::mapChanged(QMapboxGL::MapChange change)

    This signal is emitted when the state of the map has changed. This signal
    may be used for detecting errors when loading a style or detecting when
    a map is fully loaded by analyzing the parameter \a change.
*/

/*!
    \fn void QMapboxGL::mapLoadingFailed(QMapboxGL::MapLoadingFailure type, const QString &description)

    This signal is emitted when a map loading failure happens. Details of the
    failures are provided, including its \a type and textual \a description.
*/

/*!
    \fn void QMapboxGL::copyrightsChanged(const QString &copyrightsHtml);

    This signal is emitted when the copyrights of the current content of the map
    have changed. This can be caused by a style change or adding a new source.

    \a copyrightsHtml is a string with a HTML snippet.
*/

QMapboxGLPrivate::QMapboxGLPrivate(QMapboxGL *q, const QMapboxGLSettings &settings, const QSize &size, qreal pixelRatio_)
    : QObject(q)
    , m_fileSourceObj(sharedDefaultFileSource(
        settings.cacheDatabasePath().toStdString(),
        settings.assetPath().toStdString(),
        settings.cacheDatabaseMaximumSize()))
    , m_threadPool(mbgl::sharedThreadPool())
    , m_mode(settings.contextMode())
    , m_pixelRatio(pixelRatio_)
{
    // Setup the FileSource
    m_fileSourceObj->setAccessToken(settings.accessToken().toStdString());
    m_fileSourceObj->setAPIBaseURL(settings.apiBaseUrl().toStdString());

    if (settings.resourceTransform()) {
        m_resourceTransform = std::make_unique<mbgl::Actor<mbgl::ResourceTransform>>(*mbgl::Scheduler::GetCurrent(),
            [callback = settings.resourceTransform()] (mbgl::Resource::Kind, const std::string &&url_) -> std::string {
                return callback(std::move(url_));
            });
       m_fileSourceObj->setResourceTransform(m_resourceTransform->self());
    }

    // Setup MapObserver
    m_mapObserver = std::make_unique<QMapboxGLMapObserver>(this);

    qRegisterMetaType<QMapboxGL::MapChange>("QMapboxGL::MapChange");

    connect(m_mapObserver.get(), SIGNAL(mapChanged(QMapboxGL::MapChange)), q, SIGNAL(mapChanged(QMapboxGL::MapChange)));
    connect(m_mapObserver.get(), SIGNAL(mapLoadingFailed(QMapboxGL::MapLoadingFailure,QString)), q, SIGNAL(mapLoadingFailed(QMapboxGL::MapLoadingFailure,QString)));
    connect(m_mapObserver.get(), SIGNAL(copyrightsChanged(QString)), q, SIGNAL(copyrightsChanged(QString)));

    // Setup the Map object
    mapObj = std::make_unique<mbgl::Map>(
            *this, // RendererFrontend
            *m_mapObserver,
            sanitizedSize(size),
            m_pixelRatio, *m_fileSourceObj, *m_threadPool,
            static_cast<mbgl::MapMode>(settings.mapMode()),
            static_cast<mbgl::ConstrainMode>(settings.constrainMode()),
            static_cast<mbgl::ViewportMode>(settings.viewportMode()));

    // Needs to be Queued to give time to discard redundant draw calls via the `renderQueued` flag.
    connect(this, SIGNAL(needsRendering()), q, SIGNAL(needsRendering()), Qt::QueuedConnection);
}

QMapboxGLPrivate::~QMapboxGLPrivate()
{
}

void QMapboxGLPrivate::update(std::shared_ptr<mbgl::UpdateParameters> parameters)
{
    std::lock_guard<std::recursive_mutex> lock(m_mapRendererMutex);

    if (!m_mapRenderer) {
        return;
    }

    m_mapRenderer->updateParameters(std::move(parameters));

    requestRendering();
}

void QMapboxGLPrivate::setObserver(mbgl::RendererObserver &observer)
{
    m_rendererObserver = std::make_shared<QMapboxGLRendererObserver>(
            *mbgl::util::RunLoop::Get(), observer);

    std::lock_guard<std::recursive_mutex> lock(m_mapRendererMutex);

    if (m_mapRenderer) {
        m_mapRenderer->setObserver(m_rendererObserver);
    }
}

void QMapboxGLPrivate::createRenderer()
{
    std::lock_guard<std::recursive_mutex> lock(m_mapRendererMutex);

    if (m_mapRenderer) {
        return;
    }

    m_mapRenderer = std::make_unique<QMapboxGLMapRenderer>(
        m_pixelRatio,
        *m_fileSourceObj,
        *m_threadPool,
        m_mode
    );

    connect(m_mapRenderer.get(), SIGNAL(needsRendering()), this, SLOT(requestRendering()));

    m_mapRenderer->setObserver(m_rendererObserver);
}

void QMapboxGLPrivate::destroyRenderer()
{
    std::lock_guard<std::recursive_mutex> lock(m_mapRendererMutex);

    m_mapRenderer.reset();
}

void QMapboxGLPrivate::render()
{
    std::lock_guard<std::recursive_mutex> lock(m_mapRendererMutex);

    if (!m_mapRenderer) {
        createRenderer();
    }

#if defined(__APPLE__) && QT_VERSION < 0x050000
    // FIXME Qt 4.x provides an incomplete FBO at start.
    // See https://bugreports.qt.io/browse/QTBUG-36802 for details.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return;
    }
#endif

    m_renderQueued.clear();
    m_mapRenderer->render();
}

void QMapboxGLPrivate::setFramebufferObject(quint32 fbo, const QSize& size)
{
    std::lock_guard<std::recursive_mutex> lock(m_mapRendererMutex);

    if (!m_mapRenderer) {
        createRenderer();
    }

    m_mapRenderer->updateFramebuffer(fbo, sanitizedSize(size));
}

void QMapboxGLPrivate::requestRendering()
{
    if (!m_renderQueued.test_and_set()) {
        emit needsRendering();
    }
}
