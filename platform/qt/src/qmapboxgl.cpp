#include "qmapboxgl_p.hpp"
#include "make_property_setters.hpp"

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/traits.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/tileset.hpp>

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <QWindow>
#else
#include <QCoreApplication>
#endif

#include <QImage>
#include <QMapboxGL>
#include <QMargins>
#include <QString>
#include <QStringList>
#include <QThreadStorage>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

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

auto fromQStringList(const QStringList &list)
{
    std::vector<std::string> strings(list.size());
    for (const QString &string : list) {
        strings.emplace_back(string.toStdString());
    }
    return strings;
}

mbgl::Value fromQMapboxFilterValueList(const QMapbox::FilterValue &value);
mbgl::Value fromQMapboxFilterValueMap(const QMapbox::FilterValue &value);

mbgl::Value fromQMapboxFilterValue(const QMapbox::FilterValue &value) {
    switch (value.type) {
    case NullFilterValueType:
        return {};
    case BooleanFilterValueType:
        return value.value.toBool();
    case UnsignedIntegerFilterValueType:
        return value.value.value<uint64_t>();
    case SignedIntegerFilterValueType:
        return value.value.value<int64_t>();
    case DoubleFilterValueType:
        return value.value.toDouble();
    case StringFilterValueType:
        return value.value.toString().toStdString();
    case ListFilterValueType:
        return fromQMapboxFilterValueList(value);
    case MapFilterValueType:
        return fromQMapboxFilterValueMap(value);
    }
    return {};
}

mbgl::Value fromQMapboxFilterValueList(const QMapbox::FilterValue &value) {
    const auto list = value.value.value<QMapbox::FilterValueList>();
    std::vector<mbgl::Value> values;
    for (const auto& listValue : list) {
        values.emplace_back(fromQMapboxFilterValue(listValue));
    }
    return values;
}

mbgl::Value fromQMapboxFilterValueMap(const QMapbox::FilterValue &value) {
    const auto map = value.value.value<QMapbox::FilterValueMap>();
    std::unordered_map<std::string, mbgl::Value> values;
    for (auto it = map.begin(); it != map.end(); ++it) {
        values.emplace(std::make_pair(it.key().toStdString(), fromQMapboxFilterValue(it.value())));
    }
    return values;
}

template <typename FilterType>
mbgl::style::Filter fromQMapboxBinaryFilterForm(const QMapbox::Filter &filter) {
    const auto form = filter.form.value<QMapbox::BinaryFilterForm>();
    return FilterType { form.key.toStdString(), fromQMapboxFilterValue(form.value) };
}

template <typename FilterType>
mbgl::style::Filter fromQMapboxSetFilterForm(const QMapbox::Filter &filter) {
    const auto form = filter.form.value<QMapbox::SetFilterForm>();
    std::vector<mbgl::Value> values;
    for (const auto& listValue : form.values) {
        values.emplace_back(fromQMapboxFilterValue(listValue));
    }
    return FilterType { form.key.toStdString(), values };
}

mbgl::style::Filter fromQMapboxFilter(const QMapbox::Filter &filter);

template <typename FilterType>
mbgl::style::Filter fromQMapboxCompoundFilterForm(const QMapbox::Filter &filter) {
    const auto form = filter.form.value<QMapbox::CompoundFilterForm>();
    std::vector<mbgl::style::Filter> filters;
    for (const auto& compoundFilter : form.filters) {
        filters.emplace_back(fromQMapboxFilter(compoundFilter));
    }
    return FilterType { filters };
}

template <typename FilterType>
mbgl::style::Filter fromQMapboxUnaryFilterForm(const QMapbox::Filter &filter) {
    const auto form = filter.form.value<QMapbox::UnaryFilterForm>();
    return FilterType { form.key.toStdString() };
}

mbgl::style::Filter fromQMapboxFilter(const QMapbox::Filter &filter)
{
    switch (filter.type) {
    case EqualsFilterType:
        return fromQMapboxBinaryFilterForm<mbgl::style::EqualsFilter>(filter);
    case NotEqualsFilterType:
        return fromQMapboxBinaryFilterForm<mbgl::style::NotEqualsFilter>(filter);
    case GreaterThanFilterType:
        return fromQMapboxBinaryFilterForm<mbgl::style::GreaterThanFilter>(filter);
    case GreaterThanEqualsFilterType:
        return fromQMapboxBinaryFilterForm<mbgl::style::GreaterThanEqualsFilter>(filter);
    case LessThanFilterType:
        return fromQMapboxBinaryFilterForm<mbgl::style::LessThanFilter>(filter);
    case LessThanEqualsFilterType:
        return fromQMapboxBinaryFilterForm<mbgl::style::LessThanEqualsFilter>(filter);
    case InFilterType:
        return fromQMapboxSetFilterForm<mbgl::style::InFilter>(filter);
    case NotInFilterType:
        return fromQMapboxSetFilterForm<mbgl::style::NotInFilter>(filter);
    case AllFilterType:
        return fromQMapboxCompoundFilterForm<mbgl::style::AllFilter>(filter);
    case AnyFilterType:
        return fromQMapboxCompoundFilterForm<mbgl::style::AnyFilter>(filter);
    case NoneFilterType:
        return fromQMapboxCompoundFilterForm<mbgl::style::NoneFilter>(filter);
    case HasFilterType:
        return fromQMapboxUnaryFilterForm<mbgl::style::HasFilter>(filter);
    case NotHasFilterType:
        return fromQMapboxUnaryFilterForm<mbgl::style::NotHasFilter>(filter);
    }
    return {};
}

template <typename LayerType>
std::unique_ptr<LayerType> fromQMapboxVectorLayer(const QMapbox::Layer &layer) {
    auto mbglLayer = std::make_unique<LayerType>(layer.layerID.toStdString(), layer.sourceID.toString().toStdString());
    if (layer.sourceLayer.isValid()) {
        mbglLayer->setSourceLayer(layer.sourceLayer.toString().toStdString());
    }
    if (layer.filter.isValid()) {
        mbglLayer->setFilter(fromQMapboxFilter(layer.filter.value<QMapbox::Filter>()));
    }
    return mbglLayer;
}

auto fromQMapboxRasterLayer(const QMapbox::Layer &layer) {
    return std::make_unique<mbgl::style::RasterLayer>(layer.layerID.toStdString(),
                                                      layer.sourceID.toString().toStdString());
}

auto fromQMapboxBackgroundLayer(const QMapbox::Layer &layer) {
    return std::make_unique<mbgl::style::BackgroundLayer>(layer.layerID.toStdString());
}

auto fromQMapboxSourceUrlOrTileset(const QMapbox::SourceUrlOrTileset& urlOrTileset) {
    mbgl::variant<std::string, mbgl::Tileset> mbglUrlOrTileset;
    if (urlOrTileset.canConvert<QString>()) {
        mbglUrlOrTileset = urlOrTileset.toString().toStdString();
    } else {
        QMapbox::Tileset tileset = urlOrTileset.value<QMapbox::Tileset>();
        std::vector<std::string> mbglTiles;
        for (const auto& tile : tileset.tiles) {
            mbglTiles.emplace_back(tile.toStdString());
        }

        mbgl::Tileset mbglTileset;
        mbglTileset.tiles = mbglTiles;

        if (tileset.minzoom.isValid() || tileset.maxzoom.isValid()) {
            if (tileset.minzoom.isValid()) {
                mbglTileset.zoomRange.min = tileset.minzoom.value<uint8_t>();
            }
            if (tileset.maxzoom.isValid()) {
                mbglTileset.zoomRange.max = tileset.maxzoom.value<uint8_t>();
            }
        }

        if (tileset.attribution.isValid()) {
            mbglTileset.attribution = tileset.attribution.toString().toStdString();
        }

        mbglUrlOrTileset = mbglTileset;
    }

    return mbglUrlOrTileset;
}

auto fromQMapboxRasterSource(const QMapbox::Source &source) {
    return std::make_unique<mbgl::style::RasterSource>(
            source.sourceID.toStdString(),
            fromQMapboxSourceUrlOrTileset(source.urlOrTileset),
            source.tileSize.value<uint16_t>());
}

auto fromQMapboxVectorSource(const QMapbox::Source &source) {
    return std::make_unique<mbgl::style::VectorSource>(
            source.sourceID.toStdString(),
            fromQMapboxSourceUrlOrTileset(source.urlOrTileset));
}

auto fromQMapboxGeoJSONSource(const QMapbox::Source &source) {
    auto mbglSource = std::make_unique<mbgl::style::GeoJSONSource>(source.sourceID.toStdString());

    if (source.urlOrTileset.canConvert<QString>()) {
        mbglSource->setURL(source.urlOrTileset.toString().toStdString());
    } else {
        // FIXME Support inline GeoJSON (use rapidjson?).
    }

    return mbglSource;
}

} // anonymous namespace

QMapboxGLSettings::QMapboxGLSettings()
    : m_mapMode(QMapboxGLSettings::ContinuousMap)
    , m_contextMode(QMapboxGLSettings::SharedGLContext)
    , m_constrainMode(QMapboxGLSettings::ConstrainHeightOnly)
    , m_viewportMode(QMapboxGLSettings::DefaultViewport)
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

QMapboxGL::QMapboxGL(QObject *parent_, const QMapboxGLSettings &settings)
    : QObject(parent_)
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

void QMapboxGL::addClass(const QString &className, const QMapbox::TransitionOptions &options)
{
    d_ptr->mapObj->addClass(className.toStdString(), fromQMapboxTransitionOptions(options));
}

void QMapboxGL::removeClass(const QString &className)
{
    d_ptr->mapObj->removeClass(className.toStdString());
}

void QMapboxGL::removeClass(const QString &className, const QMapbox::TransitionOptions &options)
{
    d_ptr->mapObj->removeClass(className.toStdString(), fromQMapboxTransitionOptions(options));
}

bool QMapboxGL::hasClass(const QString &className) const
{
    return d_ptr->mapObj->hasClass(className.toStdString());
}

void QMapboxGL::setClasses(const QStringList &classNames)
{
    d_ptr->mapObj->setClasses(fromQStringList(classNames));
}

void QMapboxGL::setClasses(const QStringList &classNames, const QMapbox::TransitionOptions &options)
{
    d_ptr->mapObj->setClasses(fromQStringList(classNames), fromQMapboxTransitionOptions(options));
}

QStringList QMapboxGL::getClasses() const
{
    QStringList classNames;
    for (const std::string &mbglClass : d_ptr->mapObj->getClasses()) {
        classNames << QString::fromStdString(mbglClass);
    }
    return classNames;
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

    d_ptr->mapObj->addAnnotationIcon(name.toStdString(), std::make_shared<mbgl::SpriteImage>(
        mbgl::PremultipliedImage { size_t(swapped.width()), size_t(swapped.height()), std::move(img) }, 1.0));
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

void QMapboxGL::setFilter(const QString &layerID, const QMapbox::Filter &filter) {
    mbgl::style::Layer *layer = d_ptr->mapObj->getLayer(layerID.toStdString());
    if (!layer) return;

    mbgl::style::Filter mbglFilter(fromQMapboxFilter(filter));
    if (layer->is<mbgl::style::FillLayer>()) {
        layer->as<mbgl::style::FillLayer>()->setFilter(mbglFilter);
    } else if (layer->is<mbgl::style::LineLayer>()) {
        layer->as<mbgl::style::LineLayer>()->setFilter(mbglFilter);
    } else if (layer->is<mbgl::style::SymbolLayer>()) {
        layer->as<mbgl::style::SymbolLayer>()->setFilter(mbglFilter);
    } else if (layer->is<mbgl::style::CircleLayer>()) {
        layer->as<mbgl::style::CircleLayer>()->setFilter(mbglFilter);
    }
}

void QMapboxGL::addLayer(const QMapbox::Layer &layer) {
    std::unique_ptr<mbgl::style::Layer> mbglLayer;
    switch (layer.type) {
    case FillLayer:
        mbglLayer = fromQMapboxVectorLayer<mbgl::style::FillLayer>(layer);
        break;
    case LineLayer:
        mbglLayer = fromQMapboxVectorLayer<mbgl::style::LineLayer>(layer);
        break;
    case CircleLayer:
        mbglLayer = fromQMapboxVectorLayer<mbgl::style::CircleLayer>(layer);
        break;
    case SymbolLayer:
        mbglLayer = fromQMapboxVectorLayer<mbgl::style::SymbolLayer>(layer);
        break;
    case RasterLayer:
        mbglLayer = fromQMapboxRasterLayer(layer);
        break;
    case BackgroundLayer:
        mbglLayer = fromQMapboxBackgroundLayer(layer);
        break;
    }

    if (layer.minZoom.isValid()) {
        mbglLayer->setMinZoom(layer.minZoom.toFloat());
    }

    if (layer.maxZoom.isValid()) {
        mbglLayer->setMaxZoom(layer.maxZoom.toFloat());
    }

    d_ptr->mapObj->addLayer(std::move(mbglLayer));
}

void QMapboxGL::removeLayer(const QString &layerID) {
    d_ptr->mapObj->removeLayer(layerID.toStdString());
}

void QMapboxGL::addSource(const QMapbox::Source &source) {
    std::unique_ptr<mbgl::style::Source> mbglSource;
    switch (source.type) {
    case RasterSource:
        mbglSource = fromQMapboxRasterSource(source);
        break;
    case VectorSource:
        mbglSource = fromQMapboxVectorSource(source);
        break;
    case GeoJSONSource:
        mbglSource = fromQMapboxGeoJSONSource(source);
        break;
    }

    d_ptr->mapObj->addSource(std::move(mbglSource));
}

void QMapboxGL::removeSource(const QString &sourceID) {
    d_ptr->mapObj->removeSource(sourceID.toStdString());
}

void QMapboxGL::setPaintProperty(const QString &layerID, QMapbox::PaintPropertyType type, const QMapbox::PropertyValue &value, const QString &klass) {
    mbgl::style::Layer *layer = d_ptr->mapObj->getLayer(layerID.toStdString());
    if (!layer) {
        return;
    }

    static const auto setters = mbgl::style::conversion::makeQtPaintPropertySetters();
    auto it = setters.find(type);
    if (it == setters.end()) {
        return;
    }

    it->second(*layer, value, klass.toStdString());

    d_ptr->mapObj->update(mbgl::Update::RecalculateStyle);
}

void QMapboxGL::setLayoutProperty(const QString &layerID, QMapbox::LayoutPropertyType type, const QMapbox::PropertyValue &value) {
    mbgl::style::Layer *layer = d_ptr->mapObj->getLayer(layerID.toStdString());
    if (!layer) {
        return;
    }

    static const auto setters = mbgl::style::conversion::makeQtLayoutPropertySetters();
    auto it = setters.find(type);
    if (it == setters.end()) {
        return;
    }

    it->second(*layer, value);

    d_ptr->mapObj->update(mbgl::Update::RecalculateStyle);
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
