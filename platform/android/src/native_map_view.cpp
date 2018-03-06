#include "native_map_view.hpp"

#include <cstdlib>
#include <ctime>
#include <cassert>
#include <memory>
#include <list>
#include <tuple>

#include <sys/system_properties.h>

#include <android/native_window_jni.h>

#include <jni/jni.hpp>

#include <mbgl/math/minmax.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/event.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/shared_thread_pool.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/renderer/query.hpp>

// Java -> C++ conversion
#include "style/android_conversion.hpp"
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/filter.hpp>

// C++ -> Java conversion
#include "conversion/conversion.hpp"
#include "conversion/collection.hpp"
#include "style/conversion/filter.hpp"
#include "geojson/conversion/feature.hpp"

#include "jni.hpp"
#include "attach_env.hpp"
#include "map_renderer.hpp"
#include "android_renderer_frontend.hpp"
#include "file_source.hpp"
#include "bitmap.hpp"
#include "run_loop_impl.hpp"
#include "java/util.hpp"
#include "geometry/lat_lng_bounds.hpp"
#include "map/camera_position.hpp"
#include  "map/image.hpp"
#include "style/light.hpp"
#include "bitmap_factory.hpp"

namespace mbgl {
namespace android {

NativeMapView::NativeMapView(jni::JNIEnv& _env,
                             jni::Object<NativeMapView> _obj,
                             jni::Object<FileSource> jFileSource,
                             jni::Object<MapRenderer> jMapRenderer,
                             jni::jfloat _pixelRatio)
    : javaPeer(_obj.NewWeakGlobalRef(_env))
    , mapRenderer(MapRenderer::getNativePeer(_env, jMapRenderer))
    , pixelRatio(_pixelRatio)
    , threadPool(sharedThreadPool()) {

    // Get a reference to the JavaVM for callbacks
    if (_env.GetJavaVM(&vm) < 0) {
        _env.ExceptionDescribe();
        return;
    }

    // Get native peer for file source
    mbgl::FileSource& fileSource = mbgl::android::FileSource::getDefaultFileSource(_env, jFileSource);

    // Create a renderer frontend
    rendererFrontend = std::make_unique<AndroidRendererFrontend>(mapRenderer);

    // Create the core map
    map = std::make_unique<mbgl::Map>(*rendererFrontend, *this,
                                      mbgl::Size{ static_cast<uint32_t>(width),
                                                  static_cast<uint32_t>(height) }, pixelRatio,
                                      fileSource, *threadPool, MapMode::Continuous,
                                      ConstrainMode::HeightOnly, ViewportMode::Default);
}

/**
 * Called through NativeMapView#destroy()
 */
NativeMapView::~NativeMapView() {
    map.reset();
    vm = nullptr;
}

/**
 * From mbgl::RendererBackend. Callback to java NativeMapView#onMapChanged(int).
 *
 * May be called from any thread
 */
void NativeMapView::notifyMapChange(mbgl::MapChange change) {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto onMapChanged = javaClass.GetMethod<void (int)>(*_env, "onMapChanged");
    javaPeer->Call(*_env, onMapChanged, (int) change);
}

void NativeMapView::onCameraWillChange(MapObserver::CameraChangeMode mode) {
    if (mode == MapObserver::CameraChangeMode::Immediate) {
        notifyMapChange(MapChange::MapChangeRegionWillChange);
    } else {
        notifyMapChange(MapChange::MapChangeRegionWillChangeAnimated);
    }
}

void NativeMapView::onCameraIsChanging() {
    notifyMapChange(MapChange::MapChangeRegionIsChanging);
}

void NativeMapView::onCameraDidChange(MapObserver::CameraChangeMode mode) {
    if (mode == MapObserver::CameraChangeMode::Immediate) {
        notifyMapChange(MapChange::MapChangeRegionDidChange);
    } else {
        notifyMapChange(MapChange::MapChangeRegionDidChangeAnimated);
    }
}

void NativeMapView::onWillStartLoadingMap() {
    notifyMapChange(MapChange::MapChangeWillStartLoadingMap);
}

void NativeMapView::onDidFinishLoadingMap() {
    notifyMapChange(MapChange::MapChangeDidFinishLoadingMap);
}

void NativeMapView::onDidFailLoadingMap(std::exception_ptr) {
    notifyMapChange(MapChange::MapChangeDidFailLoadingMap);
}

void NativeMapView::onWillStartRenderingFrame() {
    notifyMapChange(MapChange::MapChangeWillStartRenderingFrame);
}

void NativeMapView::onDidFinishRenderingFrame(MapObserver::RenderMode mode) {
    if (mode == MapObserver::RenderMode::Partial) {
        notifyMapChange(MapChange::MapChangeDidFinishRenderingFrame);
    } else {
        notifyMapChange(MapChange::MapChangeDidFinishRenderingFrameFullyRendered);
    }
}

void NativeMapView::onWillStartRenderingMap() {
    notifyMapChange(MapChange::MapChangeWillStartRenderingMap);
}

void NativeMapView::onDidFinishRenderingMap(MapObserver::RenderMode mode) {
    if (mode == MapObserver::RenderMode::Partial) {
        notifyMapChange(MapChange::MapChangeDidFinishRenderingMap);
    } else {
        notifyMapChange(MapChange::MapChangeDidFinishRenderingMapFullyRendered);
    }
}

void NativeMapView::onDidFinishLoadingStyle() {
    notifyMapChange(MapChange::MapChangeDidFinishLoadingStyle);
}

void NativeMapView::onSourceChanged(mbgl::style::Source&) {
    notifyMapChange(MapChange::MapChangeSourceDidChange);
}

// JNI Methods //

void NativeMapView::resizeView(jni::JNIEnv&, int w, int h) {
    width = util::max(64, w);
    height = util::max(64, h);
    map->setSize({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
}

jni::String NativeMapView::getStyleUrl(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, map->getStyle().getURL());
}

void NativeMapView::setStyleUrl(jni::JNIEnv& env, jni::String url) {
    map->getStyle().loadURL(jni::Make<std::string>(env, url));
}

jni::String NativeMapView::getStyleJson(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, map->getStyle().getJSON());
}

void NativeMapView::setStyleJson(jni::JNIEnv& env, jni::String json) {
    map->getStyle().loadJSON(jni::Make<std::string>(env, json));
}

void NativeMapView::setLatLngBounds(jni::JNIEnv& env, jni::Object<mbgl::android::LatLngBounds> jBounds) {
    if (jBounds) {
        map->setLatLngBounds(mbgl::android::LatLngBounds::getLatLngBounds(env, jBounds));
    } else {
        map->setLatLngBounds(mbgl::LatLngBounds::world());
    }
}

void NativeMapView::cancelTransitions(jni::JNIEnv&) {
    map->cancelTransitions();
}

void NativeMapView::setGestureInProgress(jni::JNIEnv&, jni::jboolean inProgress) {
    map->setGestureInProgress(inProgress);
}

void NativeMapView::moveBy(jni::JNIEnv&, jni::jdouble dx, jni::jdouble dy, jni::jlong duration) {
    mbgl::AnimationOptions animationOptions;
    if (duration > 0) {
       animationOptions.duration.emplace(mbgl::Milliseconds(duration));
       animationOptions.easing.emplace(mbgl::util::UnitBezier {0.25, 0.46, 0.45, 0.94});
    }
    map->moveBy({dx, dy}, animationOptions);
}

void NativeMapView::jumpTo(jni::JNIEnv&, jni::jdouble angle, jni::jdouble latitude, jni::jdouble longitude, jni::jdouble pitch, jni::jdouble zoom) {
    mbgl::CameraOptions options;
    if (angle != -1) {
        options.angle = -angle * util::DEG2RAD;
    }
    options.center = mbgl::LatLng(latitude, longitude);
    options.padding = insets;
    if (pitch != -1) {
        options.pitch = pitch * util::DEG2RAD;
    }
    if (zoom != -1) {
        options.zoom = zoom;
    }

    map->jumpTo(options);
}

void NativeMapView::easeTo(jni::JNIEnv&, jni::jdouble angle, jni::jdouble latitude, jni::jdouble longitude, jni::jlong duration, jni::jdouble pitch, jni::jdouble zoom, jni::jboolean easing) {
    mbgl::CameraOptions cameraOptions;
    if (angle != -1) {
        cameraOptions.angle = -angle * util::DEG2RAD;
    }
    cameraOptions.center = mbgl::LatLng(latitude, longitude);
    cameraOptions.padding = insets;
    if (pitch != -1) {
        cameraOptions.pitch = pitch * util::DEG2RAD;
    }
    if (zoom != -1) {
        cameraOptions.zoom = zoom;
    }

    mbgl::AnimationOptions animationOptions;
    animationOptions.duration.emplace(mbgl::Milliseconds(duration));
    if (!easing) {
       // add a linear interpolator instead of easing
       animationOptions.easing.emplace(mbgl::util::UnitBezier { 0, 0, 1, 1 });
    }

    map->easeTo(cameraOptions, animationOptions);
}

void NativeMapView::flyTo(jni::JNIEnv&, jni::jdouble angle, jni::jdouble latitude, jni::jdouble longitude, jni::jlong duration, jni::jdouble pitch, jni::jdouble zoom) {
    mbgl::CameraOptions cameraOptions;
    if (angle != -1) {
        cameraOptions.angle = -angle * util::DEG2RAD;
    }
    cameraOptions.center = mbgl::LatLng(latitude, longitude);
    cameraOptions.padding = insets;
    if (pitch != -1) {
        cameraOptions.pitch = pitch * util::DEG2RAD;
    }
    if (zoom != -1) {
        cameraOptions.zoom = zoom;
    }

    mbgl::AnimationOptions animationOptions;
    animationOptions.duration.emplace(mbgl::Milliseconds(duration));
    map->flyTo(cameraOptions, animationOptions);
}

jni::Object<LatLng> NativeMapView::getLatLng(JNIEnv& env) {
    return LatLng::New(env, map->getLatLng(insets));
}

void NativeMapView::setLatLng(jni::JNIEnv&, jni::jdouble latitude, jni::jdouble longitude, jni::jlong duration) {
    map->setLatLng(mbgl::LatLng(latitude, longitude), insets, mbgl::AnimationOptions{mbgl::Milliseconds(duration)});
}

jni::Object<CameraPosition> NativeMapView::getCameraForLatLngBounds(jni::JNIEnv& env, jni::Object<LatLngBounds> jBounds) {
    return CameraPosition::New(env, map->cameraForLatLngBounds(mbgl::android::LatLngBounds::getLatLngBounds(env, jBounds), insets));
}

jni::Object<CameraPosition> NativeMapView::getCameraForGeometry(jni::JNIEnv& env, jni::Object<geojson::Geometry> jGeometry, double bearing) {
    auto geometry = geojson::Geometry::convert(env, jGeometry);
    return CameraPosition::New(env, map->cameraForGeometry(geometry, insets, bearing));
}

void NativeMapView::setReachability(jni::JNIEnv&, jni::jboolean reachable) {
    if (reachable) {
        mbgl::NetworkStatus::Reachable();
    }
}

void NativeMapView::resetPosition(jni::JNIEnv&) {
    map->resetPosition();
}

jni::jdouble NativeMapView::getPitch(jni::JNIEnv&) {
    return map->getPitch();
}

void NativeMapView::setPitch(jni::JNIEnv&, jni::jdouble pitch, jni::jlong duration) {
    map->setPitch(pitch, mbgl::AnimationOptions{mbgl::Milliseconds(duration)});
}

void NativeMapView::setZoom(jni::JNIEnv&, jni::jdouble zoom, jni::jdouble x, jni::jdouble y, jni::jlong duration) {
    map->setZoom(zoom, mbgl::ScreenCoordinate{x,y}, mbgl::AnimationOptions{mbgl::Milliseconds(duration)});
}

jni::jdouble NativeMapView::getZoom(jni::JNIEnv&) {
    return map->getZoom();
}

void NativeMapView::resetZoom(jni::JNIEnv&) {
    map->resetZoom();
}

void NativeMapView::setMinZoom(jni::JNIEnv&, jni::jdouble zoom) {
    map->setMinZoom(zoom);
}

jni::jdouble NativeMapView::getMinZoom(jni::JNIEnv&) {
    return map->getMinZoom();
}

void NativeMapView::setMaxZoom(jni::JNIEnv&, jni::jdouble zoom) {
    map->setMaxZoom(zoom);
}

jni::jdouble NativeMapView::getMaxZoom(jni::JNIEnv&) {
    return map->getMaxZoom();
}

void NativeMapView::rotateBy(jni::JNIEnv&, jni::jdouble sx, jni::jdouble sy, jni::jdouble ex, jni::jdouble ey, jni::jlong duration) {
    mbgl::ScreenCoordinate first(sx, sy);
    mbgl::ScreenCoordinate second(ex, ey);
    map->rotateBy(first, second, mbgl::AnimationOptions{mbgl::Milliseconds(duration)});
}

void NativeMapView::setBearing(jni::JNIEnv&, jni::jdouble degrees, jni::jlong duration) {
    map->setBearing(degrees, mbgl::AnimationOptions{mbgl::Milliseconds(duration)});
}

void NativeMapView::setBearingXY(jni::JNIEnv&, jni::jdouble degrees, jni::jdouble cx, jni::jdouble cy, jni::jlong duration) {
    mbgl::ScreenCoordinate center(cx, cy);
    map->setBearing(degrees, center, mbgl::AnimationOptions{mbgl::Milliseconds(duration)});
}

jni::jdouble NativeMapView::getBearing(jni::JNIEnv&) {
    return map->getBearing();
}

void NativeMapView::resetNorth(jni::JNIEnv&) {
    map->resetNorth();
}

void NativeMapView::setVisibleCoordinateBounds(JNIEnv& env, jni::Array<jni::Object<LatLng>> coordinates, jni::Object<RectF> padding, jdouble direction, jni::jlong duration) {
    NullCheck(env, &coordinates);
    std::size_t count = coordinates.Length(env);

    std::vector<mbgl::LatLng> latLngs;
    latLngs.reserve(count);

    for (std::size_t i = 0; i < count; i++) {
        auto latLng = coordinates.Get(env, i);
        latLngs.push_back(LatLng::getLatLng(env, latLng));
        jni::DeleteLocalRef(env, latLng);
    }

    mbgl::EdgeInsets mbglInsets = { RectF::getTop(env, padding), RectF::getLeft(env, padding), RectF::getBottom(env, padding), RectF::getRight(env, padding) };
    mbgl::CameraOptions cameraOptions = map->cameraForLatLngs(latLngs, mbglInsets);
    if (direction >= 0) {
        // convert from degrees to radians
        cameraOptions.angle = (-direction * M_PI) / 180;
    }

    mbgl::AnimationOptions animationOptions;
    if (duration > 0) {
        animationOptions.duration.emplace(mbgl::Milliseconds(duration));
        // equivalent to kCAMediaTimingFunctionDefault in iOS
        animationOptions.easing.emplace(mbgl::util::UnitBezier { 0.25, 0.1, 0.25, 0.1 });
    }

    map->easeTo(cameraOptions, animationOptions);
}

void NativeMapView::setContentPadding(JNIEnv&, double top, double left, double bottom, double right) {
    insets = {top, left, bottom, right};
}

void NativeMapView::scheduleSnapshot(jni::JNIEnv&) {
    mapRenderer.requestSnapshot([&](PremultipliedImage image) {
        auto _env = android::AttachEnv();
        // Convert image to bitmap
        auto bitmap = Bitmap::CreateBitmap(*_env, std::move(image));

        // invoke Mapview#OnSnapshotReady
        static auto onSnapshotReady = javaClass.GetMethod<void (jni::Object<Bitmap>)>(*_env, "onSnapshotReady");
        javaPeer->Call(*_env, onSnapshotReady, bitmap);
    });
}

jni::Object<CameraPosition> NativeMapView::getCameraPosition(jni::JNIEnv& env) {
    return CameraPosition::New(env, map->getCameraOptions(insets));
}

void NativeMapView::updateMarker(jni::JNIEnv& env, jni::jlong markerId, jni::jdouble lat, jni::jdouble lon, jni::String jid) {
    if (markerId == -1) {
        return;
    }

    std::string iconId = jni::Make<std::string>(env, jid);
    // Because Java only has int, not unsigned int, we need to bump the annotation id up to a long.
    map->updateAnnotation(markerId, mbgl::SymbolAnnotation { mbgl::Point<double>(lon, lat), iconId });
}

jni::Array<jni::jlong> NativeMapView::addMarkers(jni::JNIEnv& env, jni::Array<jni::Object<Marker>> jmarkers) {
    jni::NullCheck(env, &jmarkers);
    std::size_t len = jmarkers.Length(env);

    std::vector<jni::jlong> ids;
    ids.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        jni::Object<Marker> marker = jmarkers.Get(env, i);
        ids.push_back(map->addAnnotation(mbgl::SymbolAnnotation {
            Marker::getPosition(env, marker),
            Marker::getIconId(env, marker)
        }));

        jni::DeleteLocalRef(env, marker);
    }

    auto result = jni::Array<jni::jlong>::New(env, len);
    result.SetRegion<std::vector<jni::jlong>>(env, 0, ids);

    return result;
}

void NativeMapView::onLowMemory(JNIEnv&) {
    rendererFrontend->reduceMemoryUse();
}

using DebugOptions = mbgl::MapDebugOptions;

void NativeMapView::setDebug(JNIEnv&, jni::jboolean debug) {
    DebugOptions debugOptions = debug ? DebugOptions::TileBorders | DebugOptions::ParseStatus | DebugOptions::Collision
                                      : DebugOptions::NoDebug;
    map->setDebug(debugOptions);
}

void NativeMapView::cycleDebugOptions(JNIEnv&) {
    map->cycleDebugOptions();
}

jni::jboolean NativeMapView::getDebug(JNIEnv&) {
    return map->getDebug() != DebugOptions::NoDebug;
}

jni::jboolean NativeMapView::isFullyLoaded(JNIEnv&) {
    return map->isFullyLoaded();
}

jni::jdouble NativeMapView::getMetersPerPixelAtLatitude(JNIEnv&, jni::jdouble lat, jni::jdouble zoom) {
    return mbgl::Projection::getMetersPerPixelAtLatitude(lat, zoom);
}

jni::Object<ProjectedMeters> NativeMapView::projectedMetersForLatLng(JNIEnv& env, jni::jdouble latitude, jni::jdouble longitude) {
    mbgl::ProjectedMeters projectedMeters = mbgl::Projection::projectedMetersForLatLng(mbgl::LatLng(latitude, longitude));
    return ProjectedMeters::New(env, projectedMeters.northing(), projectedMeters.easting());
}

jni::Object<LatLng> NativeMapView::latLngForProjectedMeters(JNIEnv& env, jdouble northing, jdouble easting) {
    return LatLng::New(env, mbgl::Projection::latLngForProjectedMeters(mbgl::ProjectedMeters(northing, easting)));
}

jni::Object<PointF> NativeMapView::pixelForLatLng(JNIEnv& env, jdouble latitude, jdouble longitude) {
    mbgl::ScreenCoordinate pixel = map->pixelForLatLng(mbgl::LatLng(latitude, longitude));
    return PointF::New(env, static_cast<float>(pixel.x), static_cast<float>(pixel.y));
}

jni::Object<LatLng> NativeMapView::latLngForPixel(JNIEnv& env, jfloat x, jfloat y) {
    return LatLng::New(env, map->latLngForPixel(mbgl::ScreenCoordinate(x, y)));
}

jni::Array<jlong> NativeMapView::addPolylines(JNIEnv& env, jni::Array<jni::Object<Polyline>> polylines) {
    NullCheck(env, &polylines);
    std::size_t len = polylines.Length(env);

    std::vector<jni::jlong> ids;
    ids.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        auto polyline = polylines.Get(env, i);

        mbgl::LineAnnotation annotation = Polyline::toAnnotation(env, polyline);
        ids.push_back(map->addAnnotation(annotation));

        jni::DeleteLocalRef(env, polyline);
    }

    auto result = jni::Array<jni::jlong>::New(env, len);
    result.SetRegion<std::vector<jni::jlong>>(env, 0, ids);

    return result;
}


jni::Array<jlong> NativeMapView::addPolygons(JNIEnv& env, jni::Array<jni::Object<Polygon>> polygons) {
    NullCheck(env, &polygons);
    std::size_t len = polygons.Length(env);

    std::vector<jni::jlong> ids;
    ids.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        auto polygon = polygons.Get(env, i);

        mbgl::FillAnnotation annotation = Polygon::toAnnotation(env, polygon);
        ids.push_back(map->addAnnotation(annotation));

        jni::DeleteLocalRef(env, polygon);
    }

    auto result = jni::Array<jni::jlong>::New(env, len);
    result.SetRegion<std::vector<jni::jlong>>(env, 0, ids);

    return result;
}

//TODO: Move to Polyline class and make native peer
void NativeMapView::updatePolyline(JNIEnv& env, jlong polylineId, jni::Object<Polyline> polyline) {
    mbgl::LineAnnotation annotation = Polyline::toAnnotation(env, polyline);
    map->updateAnnotation(polylineId, annotation);
}

//TODO: Move to Polygon class and make native peer
void NativeMapView::updatePolygon(JNIEnv& env, jlong polygonId, jni::Object<Polygon> polygon) {
    mbgl::FillAnnotation annotation = Polygon::toAnnotation(env, polygon);
    map->updateAnnotation(polygonId, annotation);
}

void NativeMapView::removeAnnotations(JNIEnv& env, jni::Array<jlong> ids) {
    NullCheck(env, &ids);
    std::size_t len = ids.Length(env);
    auto elements = jni::GetArrayElements(env, *ids);
    jlong* jids = std::get<0>(elements).get();

    for (std::size_t i = 0; i < len; i++) {
        if(jids[i] == -1L) {
          continue;
        }
        map->removeAnnotation(jids[i]);
    }
}

void NativeMapView::addAnnotationIcon(JNIEnv& env, jni::String symbol, jint w, jint h, jfloat scale, jni::Array<jbyte> jpixels) {
    const std::string symbolName = jni::Make<std::string>(env, symbol);

    NullCheck(env, &jpixels);
    std::size_t size = jpixels.Length(env);

    mbgl::PremultipliedImage premultipliedImage({ static_cast<uint32_t>(w), static_cast<uint32_t>(h) });
    if (premultipliedImage.bytes() != uint32_t(size)) {
        throw mbgl::util::SpriteImageException("Sprite image pixel count mismatch");
    }

    jni::GetArrayRegion(env, *jpixels, 0, size, reinterpret_cast<jbyte*>(premultipliedImage.data.get()));
    map->addAnnotationImage(std::make_unique<mbgl::style::Image>(
        symbolName, std::move(premultipliedImage), float(scale)));
}

void NativeMapView::removeAnnotationIcon(JNIEnv& env, jni::String symbol) {
    const std::string symbolName = jni::Make<std::string>(env, symbol);
    map->removeAnnotationImage(symbolName);
}

jdouble NativeMapView::getTopOffsetPixelsForAnnotationSymbol(JNIEnv& env, jni::String symbolName) {
    return map->getTopOffsetPixelsForAnnotationImage(jni::Make<std::string>(env, symbolName));
}

jlong NativeMapView::getTransitionDuration(JNIEnv&) {
    const auto transitionOptions = map->getStyle().getTransitionOptions();
    return std::chrono::duration_cast<std::chrono::milliseconds>(transitionOptions.duration.value_or(mbgl::Duration::zero())).count();
}

void NativeMapView::setTransitionDuration(JNIEnv&, jlong duration) {
    auto transitionOptions = map->getStyle().getTransitionOptions();
    transitionOptions.duration.emplace(mbgl::Milliseconds(duration));
    map->getStyle().setTransitionOptions(transitionOptions);
}

jlong NativeMapView::getTransitionDelay(JNIEnv&) {
    const auto transitionOptions = map->getStyle().getTransitionOptions();
    return std::chrono::duration_cast<std::chrono::milliseconds>(transitionOptions.delay.value_or(mbgl::Duration::zero())).count();
}

void NativeMapView::setTransitionDelay(JNIEnv&, jlong delay) {
    auto transitionOptions = map->getStyle().getTransitionOptions();
    transitionOptions.delay.emplace(mbgl::Milliseconds(delay));
    map->getStyle().setTransitionOptions(transitionOptions);
}

jni::Array<jlong> NativeMapView::queryPointAnnotations(JNIEnv& env, jni::Object<RectF> rect) {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    // Convert input
    mbgl::ScreenBox box = {
        { RectF::getLeft(env, rect), RectF::getTop(env, rect) },
        { RectF::getRight(env, rect), RectF::getBottom(env, rect) },
    };

    // Assume only points for now
    mbgl::AnnotationIDs ids = rendererFrontend->queryPointAnnotations(box);

    // Convert result
    std::vector<jlong> longIds(ids.begin(), ids.end());
    auto result = jni::Array<jni::jlong>::New(env, ids.size());
    result.SetRegion<std::vector<jni::jlong>>(env, 0, longIds);

    return result;
}

jni::Array<jlong> NativeMapView::queryShapeAnnotations(JNIEnv &env, jni::Object<RectF> rect) {
    using namespace mbgl::style;
    using namespace mbgl::style::conversion;

    // Convert input
    mbgl::ScreenBox box = {
         {RectF::getLeft(env, rect),  RectF::getTop(env, rect)},
         {RectF::getRight(env, rect), RectF::getBottom(env, rect)},
    };

    mbgl::AnnotationIDs ids = rendererFrontend->queryShapeAnnotations(box);

    // Convert result
    std::vector<jlong> longIds(ids.begin(), ids.end());
    auto result = jni::Array<jni::jlong>::New(env, ids.size());
    result.SetRegion<std::vector<jni::jlong>>(env, 0, longIds);

    return result;
}

jni::Array<jni::Object<geojson::Feature>> NativeMapView::queryRenderedFeaturesForPoint(JNIEnv& env, jni::jfloat x, jni::jfloat y,
                                                                              jni::Array<jni::String> layerIds,
                                                                              jni::Array<jni::Object<>> jfilter) {
    using namespace mbgl::android::conversion;
    using namespace mbgl::android::geojson;

    mbgl::optional<std::vector<std::string>> layers;
    if (layerIds != nullptr && layerIds.Length(env) > 0) {
        layers = android::conversion::toVector(env, layerIds);
    }
    mapbox::geometry::point<double> point = {x, y};

    return *convert<jni::Array<jni::Object<Feature>>, std::vector<mbgl::Feature>>(
            env,
            rendererFrontend->queryRenderedFeatures(point, { layers, toFilter(env, jfilter) }));
}

jni::Array<jni::Object<geojson::Feature>> NativeMapView::queryRenderedFeaturesForBox(JNIEnv& env, jni::jfloat left, jni::jfloat top,
                                                                            jni::jfloat right, jni::jfloat bottom, jni::Array<jni::String> layerIds,
                                                                            jni::Array<jni::Object<>> jfilter) {
    using namespace mbgl::android::conversion;
    using namespace mbgl::android::geojson;

    mbgl::optional<std::vector<std::string>> layers;
    if (layerIds != nullptr && layerIds.Length(env) > 0) {
        layers = toVector(env, layerIds);
    }
    mapbox::geometry::box<double> box = {
            mapbox::geometry::point<double>{ left, top},
            mapbox::geometry::point<double>{ right, bottom }
    };

    return *convert<jni::Array<jni::Object<Feature>>, std::vector<mbgl::Feature>>(
            env,
            rendererFrontend->queryRenderedFeatures(box, { layers, toFilter(env, jfilter) }));
}

jni::Object<Light> NativeMapView::getLight(JNIEnv& env) {
    mbgl::style::Light* light = map->getStyle().getLight();
    if (light) {
        return jni::Object<Light>(Light::createJavaLightPeer(env, *map, *light));
    } else {
        return jni::Object<Light>();
    }
}

jni::Array<jni::Object<Layer>> NativeMapView::getLayers(JNIEnv& env) {

    // Get the core layers
    std::vector<style::Layer*> layers = map->getStyle().getLayers();

    // Convert
    jni::Array<jni::Object<Layer>> jLayers = jni::Array<jni::Object<Layer>>::New(env, layers.size(), Layer::javaClass);
    int index = 0;
    for (auto layer : layers) {
        auto jLayer = jni::Object<Layer>(createJavaLayerPeer(env, *map, *layer));
        jLayers.Set(env, index, jLayer);
        jni::DeleteLocalRef(env, jLayer);
        index++;
    }

    return jLayers;
}

jni::Object<Layer> NativeMapView::getLayer(JNIEnv& env, jni::String layerId) {

    // Find the layer
    mbgl::style::Layer* coreLayer = map->getStyle().getLayer(jni::Make<std::string>(env, layerId));
    if (!coreLayer) {
       mbgl::Log::Debug(mbgl::Event::JNI, "No layer found");
       return jni::Object<Layer>();
    }

    // Create and return the layer's native peer
    return jni::Object<Layer>(createJavaLayerPeer(env, *map, *coreLayer));
}

void NativeMapView::addLayer(JNIEnv& env, jlong nativeLayerPtr, jni::String before) {
    assert(nativeLayerPtr != 0);

    Layer *layer = reinterpret_cast<Layer *>(nativeLayerPtr);
    try {
        layer->addToMap(*map, before ? mbgl::optional<std::string>(jni::Make<std::string>(env, before)) : mbgl::optional<std::string>());
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"), error.what());
    }
}

void NativeMapView::addLayerAbove(JNIEnv& env, jlong nativeLayerPtr, jni::String above) {
    assert(nativeLayerPtr != 0);

    Layer *layer = reinterpret_cast<Layer *>(nativeLayerPtr);

    // Find the sibling
    auto layers = map->getStyle().getLayers();
    auto siblingId = jni::Make<std::string>(env, above);

    size_t index = 0;
    for (auto l : layers) {
        if (l->getID() == siblingId) {
            break;
        }
        index++;
    }

    // Check if we found a sibling to place before
    mbgl::optional<std::string> before;
    if (index + 1 > layers.size()) {
        // Not found
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"),
            std::string("Could not find layer: ").append(siblingId).c_str());
        return;
    } else if (index + 1 < layers.size()) {
        // Place before the sibling
        before = { layers.at(index + 1)->getID() };
    }

    // Add the layer
    try {
        layer->addToMap(*map, before);
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"), error.what());
    }
}

void NativeMapView::addLayerAt(JNIEnv& env, jlong nativeLayerPtr, jni::jint index) {
    assert(nativeLayerPtr != 0);

    Layer *layer = reinterpret_cast<Layer *>(nativeLayerPtr);
    auto layers = map->getStyle().getLayers();

    // Check index
    int numLayers = layers.size() - 1;
    if (index > numLayers || index < 0) {
        Log::Error(Event::JNI, "Index out of range: %i", index);
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"),
            std::string("Invalid index").c_str());
        return;
    }

    // Insert it below the current at that index
    try {
        layer->addToMap(*map, layers.at(index)->getID());
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"), error.what());
    }
}

/**
 * Remove by layer id.
 */
jni::Object<Layer> NativeMapView::removeLayerById(JNIEnv& env, jni::String id) {
    std::unique_ptr<mbgl::style::Layer> coreLayer = map->getStyle().removeLayer(jni::Make<std::string>(env, id));
    if (coreLayer) {
        return jni::Object<Layer>(createJavaLayerPeer(env, *map, std::move(coreLayer)));
    } else {
        return jni::Object<Layer>();
    }
}

/**
 * Remove layer at index.
 */
jni::Object<Layer> NativeMapView::removeLayerAt(JNIEnv& env, jni::jint index) {
    auto layers = map->getStyle().getLayers();

    // Check index
    int numLayers = layers.size() - 1;
    if (index > numLayers || index < 0) {
        Log::Warning(Event::JNI, "Index out of range: %i", index);
        return jni::Object<Layer>();
    }

    std::unique_ptr<mbgl::style::Layer> coreLayer = map->getStyle().removeLayer(layers.at(index)->getID());
    if (coreLayer) {
        return jni::Object<Layer>(createJavaLayerPeer(env, *map, std::move(coreLayer)));
    } else {
        return jni::Object<Layer>();
    }
}

/**
 * Remove with wrapper object id. Ownership is transferred back to the wrapper
 */
void NativeMapView::removeLayer(JNIEnv&, jlong layerPtr) {
    assert(layerPtr != 0);

    mbgl::android::Layer *layer = reinterpret_cast<mbgl::android::Layer *>(layerPtr);
    std::unique_ptr<mbgl::style::Layer> coreLayer = map->getStyle().removeLayer(layer->get().getID());
    if (coreLayer) {
        layer->setLayer(std::move(coreLayer));
    }
}

jni::Array<jni::Object<Source>> NativeMapView::getSources(JNIEnv& env) {
    // Get the core sources
    std::vector<style::Source*> sources = map->getStyle().getSources();

    // Convert
    jni::Array<jni::Object<Source>> jSources = jni::Array<jni::Object<Source>>::New(env, sources.size(), Source::javaClass);
    int index = 0;
    for (auto source : sources) {
        jSources.Set(env, index, Source::peerForCoreSource(env, *source, *rendererFrontend));
        index++;
    }

    return jSources;
}

jni::Object<Source> NativeMapView::getSource(JNIEnv& env, jni::String sourceId) {
    // Find the source
    mbgl::style::Source* coreSource = map->getStyle().getSource(jni::Make<std::string>(env, sourceId));
    if (!coreSource) {
       mbgl::Log::Debug(mbgl::Event::JNI, "No source found");
       return jni::Object<Source>();
    }

    // Create and return the source's native peer
    return Source::peerForCoreSource(env, *coreSource, *rendererFrontend);
}

void NativeMapView::addSource(JNIEnv& env, jni::Object<Source> obj, jlong sourcePtr) {
    assert(sourcePtr != 0);

    Source *source = reinterpret_cast<Source *>(sourcePtr);
    try {
        source->addToMap(env, obj, *map, *rendererFrontend);
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/sources/CannotAddSourceException"), error.what());
    }
}

void NativeMapView::removeSource(JNIEnv& env, jni::Object<Source> obj, jlong sourcePtr) {
    assert(sourcePtr != 0);

    mbgl::android::Source *source = reinterpret_cast<mbgl::android::Source *>(sourcePtr);
    source->removeFromMap(env, obj, *map);
}

void NativeMapView::addImage(JNIEnv& env, jni::String name, jni::jint w, jni::jint h, jni::jfloat scale, jni::Array<jbyte> pixels) {
    jni::NullCheck(env, &pixels);
    std::size_t size = pixels.Length(env);

    mbgl::PremultipliedImage premultipliedImage({ static_cast<uint32_t>(w), static_cast<uint32_t>(h) });
    if (premultipliedImage.bytes() != uint32_t(size)) {
        throw mbgl::util::SpriteImageException("Sprite image pixel count mismatch");
    }

    jni::GetArrayRegion(env, *pixels, 0, size, reinterpret_cast<jbyte*>(premultipliedImage.data.get()));

    map->getStyle().addImage(std::make_unique<mbgl::style::Image>(
        jni::Make<std::string>(env, name),
        std::move(premultipliedImage),
        float(scale)));
}

void NativeMapView::addImages(JNIEnv& env, jni::Array<jni::Object<mbgl::android::Image>> jimages) {
    jni::NullCheck(env, &jimages);
    std::size_t len = jimages.Length(env);

    for (std::size_t i = 0; i < len; i++) {
        jni::Object<mbgl::android::Image> jimage = jimages.Get(env, i);
        auto image = mbgl::android::Image::getImage(env, jimage);
        map->getStyle().addImage(std::make_unique<mbgl::style::Image>(image));
        jni::DeleteLocalRef(env, jimage);
    }
}

void NativeMapView::removeImage(JNIEnv& env, jni::String name) {
    map->getStyle().removeImage(jni::Make<std::string>(env, name));
}

jni::Object<Bitmap> NativeMapView::getImage(JNIEnv& env, jni::String name) {
    const mbgl::style::Image *image = map->getStyle().getImage(jni::Make<std::string>(env, name));
    if (image) {
        return Bitmap::CreateBitmap(env, image->getImage());
    } else {
        return jni::Object<Bitmap>();
    }
}

void NativeMapView::setPrefetchesTiles(JNIEnv&, jni::jboolean enable) {
    map->setPrefetchZoomDelta(enable ? util::DEFAULT_PREFETCH_ZOOM_DELTA : uint8_t(0));
}

jni::jboolean NativeMapView::getPrefetchesTiles(JNIEnv&) {
    return jni::jboolean(map->getPrefetchZoomDelta() > 0);
}

// Static methods //

jni::Class<NativeMapView> NativeMapView::javaClass;

void NativeMapView::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    NativeMapView::javaClass = *jni::Class<NativeMapView>::Find(env).NewGlobalRef(env).release();

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<NativeMapView>(env, NativeMapView::javaClass, "nativePtr",
            std::make_unique<NativeMapView, JNIEnv&, jni::Object<NativeMapView>, jni::Object<FileSource>, jni::Object<MapRenderer>, jni::jfloat>,
            "nativeInitialize",
            "nativeDestroy",
            METHOD(&NativeMapView::resizeView, "nativeResizeView"),
            METHOD(&NativeMapView::getStyleUrl, "nativeGetStyleUrl"),
            METHOD(&NativeMapView::setStyleUrl, "nativeSetStyleUrl"),
            METHOD(&NativeMapView::getStyleJson, "nativeGetStyleJson"),
            METHOD(&NativeMapView::setStyleJson, "nativeSetStyleJson"),
            METHOD(&NativeMapView::cancelTransitions, "nativeCancelTransitions"),
            METHOD(&NativeMapView::setGestureInProgress, "nativeSetGestureInProgress"),
            METHOD(&NativeMapView::moveBy, "nativeMoveBy"),
            METHOD(&NativeMapView::jumpTo, "nativeJumpTo"),
            METHOD(&NativeMapView::easeTo, "nativeEaseTo"),
            METHOD(&NativeMapView::flyTo, "nativeFlyTo"),
            METHOD(&NativeMapView::getLatLng, "nativeGetLatLng"),
            METHOD(&NativeMapView::setLatLng, "nativeSetLatLng"),
            METHOD(&NativeMapView::getCameraForLatLngBounds, "nativeGetCameraForLatLngBounds"),
            METHOD(&NativeMapView::getCameraForGeometry, "nativeGetCameraForGeometry"),
            METHOD(&NativeMapView::setReachability, "nativeSetReachability"),
            METHOD(&NativeMapView::resetPosition, "nativeResetPosition"),
            METHOD(&NativeMapView::getPitch, "nativeGetPitch"),
            METHOD(&NativeMapView::setPitch, "nativeSetPitch"),
            METHOD(&NativeMapView::getZoom, "nativeGetZoom"),
            METHOD(&NativeMapView::setZoom, "nativeSetZoom"),
            METHOD(&NativeMapView::resetZoom, "nativeResetZoom"),
            METHOD(&NativeMapView::setMinZoom, "nativeSetMinZoom"),
            METHOD(&NativeMapView::getMinZoom, "nativeGetMinZoom"),
            METHOD(&NativeMapView::setMaxZoom, "nativeSetMaxZoom"),
            METHOD(&NativeMapView::getMaxZoom, "nativeGetMaxZoom"),
            METHOD(&NativeMapView::rotateBy, "nativeRotateBy"),
            METHOD(&NativeMapView::setBearing, "nativeSetBearing"),
            METHOD(&NativeMapView::setBearingXY, "nativeSetBearingXY"),
            METHOD(&NativeMapView::getBearing, "nativeGetBearing"),
            METHOD(&NativeMapView::resetNorth, "nativeResetNorth"),
            METHOD(&NativeMapView::setVisibleCoordinateBounds, "nativeSetVisibleCoordinateBounds"),
            METHOD(&NativeMapView::setContentPadding, "nativeSetContentPadding"),
            METHOD(&NativeMapView::scheduleSnapshot, "nativeTakeSnapshot"),
            METHOD(&NativeMapView::getCameraPosition, "nativeGetCameraPosition"),
            METHOD(&NativeMapView::updateMarker, "nativeUpdateMarker"),
            METHOD(&NativeMapView::addMarkers, "nativeAddMarkers"),
            METHOD(&NativeMapView::setDebug, "nativeSetDebug"),
            METHOD(&NativeMapView::cycleDebugOptions, "nativeCycleDebugOptions"),
            METHOD(&NativeMapView::getDebug, "nativeGetDebug"),
            METHOD(&NativeMapView::isFullyLoaded, "nativeIsFullyLoaded"),
            METHOD(&NativeMapView::onLowMemory, "nativeOnLowMemory"),
            METHOD(&NativeMapView::getMetersPerPixelAtLatitude, "nativeGetMetersPerPixelAtLatitude"),
            METHOD(&NativeMapView::projectedMetersForLatLng, "nativeProjectedMetersForLatLng"),
            METHOD(&NativeMapView::pixelForLatLng, "nativePixelForLatLng"),
            METHOD(&NativeMapView::latLngForProjectedMeters, "nativeLatLngForProjectedMeters"),
            METHOD(&NativeMapView::latLngForPixel, "nativeLatLngForPixel"),
            METHOD(&NativeMapView::addPolylines, "nativeAddPolylines"),
            METHOD(&NativeMapView::addPolygons, "nativeAddPolygons"),
            METHOD(&NativeMapView::updatePolyline, "nativeUpdatePolyline"),
            METHOD(&NativeMapView::updatePolygon, "nativeUpdatePolygon"),
            METHOD(&NativeMapView::removeAnnotations, "nativeRemoveAnnotations"),
            METHOD(&NativeMapView::addAnnotationIcon, "nativeAddAnnotationIcon"),
            METHOD(&NativeMapView::removeAnnotationIcon, "nativeRemoveAnnotationIcon"),
            METHOD(&NativeMapView::getTopOffsetPixelsForAnnotationSymbol, "nativeGetTopOffsetPixelsForAnnotationSymbol"),
            METHOD(&NativeMapView::getTransitionDuration, "nativeGetTransitionDuration"),
            METHOD(&NativeMapView::setTransitionDuration, "nativeSetTransitionDuration"),
            METHOD(&NativeMapView::getTransitionDelay, "nativeGetTransitionDelay"),
            METHOD(&NativeMapView::setTransitionDelay, "nativeSetTransitionDelay"),
            METHOD(&NativeMapView::queryPointAnnotations, "nativeQueryPointAnnotations"),
            METHOD(&NativeMapView::queryShapeAnnotations, "nativeQueryShapeAnnotations"),
            METHOD(&NativeMapView::queryRenderedFeaturesForPoint, "nativeQueryRenderedFeaturesForPoint"),
            METHOD(&NativeMapView::queryRenderedFeaturesForBox, "nativeQueryRenderedFeaturesForBox"),
            METHOD(&NativeMapView::getLight, "nativeGetLight"),
            METHOD(&NativeMapView::getLayers, "nativeGetLayers"),
            METHOD(&NativeMapView::getLayer, "nativeGetLayer"),
            METHOD(&NativeMapView::addLayer, "nativeAddLayer"),
            METHOD(&NativeMapView::addLayerAbove, "nativeAddLayerAbove"),
            METHOD(&NativeMapView::addLayerAt, "nativeAddLayerAt"),
            METHOD(&NativeMapView::removeLayerById, "nativeRemoveLayerById"),
            METHOD(&NativeMapView::removeLayerAt, "nativeRemoveLayerAt"),
            METHOD(&NativeMapView::removeLayer, "nativeRemoveLayer"),
            METHOD(&NativeMapView::getSources, "nativeGetSources"),
            METHOD(&NativeMapView::getSource, "nativeGetSource"),
            METHOD(&NativeMapView::addSource, "nativeAddSource"),
            METHOD(&NativeMapView::removeSource, "nativeRemoveSource"),
            METHOD(&NativeMapView::addImage, "nativeAddImage"),
            METHOD(&NativeMapView::addImages, "nativeAddImages"),
            METHOD(&NativeMapView::removeImage, "nativeRemoveImage"),
            METHOD(&NativeMapView::getImage, "nativeGetImage"),
            METHOD(&NativeMapView::setLatLngBounds, "nativeSetLatLngBounds"),
            METHOD(&NativeMapView::setPrefetchesTiles, "nativeSetPrefetchesTiles"),
            METHOD(&NativeMapView::getPrefetchesTiles, "nativeGetPrefetchesTiles")
    );
}

} // namespace android
} // namespace mbgl
