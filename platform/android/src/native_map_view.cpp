#include "native_map_view.hpp"

#include <cstdlib>
#include <ctime>
#include <cassert>
#include <memory>
#include <list>
#include <tuple>

#include <sys/system_properties.h>

#include <EGL/egl.h>

#include <jni/jni.hpp>

#include <mbgl/gl/context.hpp>
#include <mbgl/gl/extension.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/event.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/sprite/sprite_image.hpp>

#include "conversion/conversion.hpp"
#include "conversion/collection.hpp"
#include "geometry/conversion/feature.hpp"

#include "jni.hpp"
#include "attach_env.hpp"
#include "bitmap.hpp"
#include "run_loop_impl.hpp"
#include "java/util.hpp"

namespace mbgl {
namespace android {

using DebugOptions = mbgl::MapDebugOptions;

NativeMapView::NativeMapView(jni::JNIEnv& _env, jni::Object<NativeMapView> _obj, jni::String _cachePath, jni::String _apkPath,
                             jni::jfloat _pixelRatio, jni::jint _availableProcessors, jni::jlong _totalMemory) :
    javaPeer(_obj.NewWeakGlobalRef(_env)),
    pixelRatio(_pixelRatio),
    availableProcessors(_availableProcessors),
    totalMemory(_totalMemory),
    runLoop(std::make_unique<mbgl::util::RunLoop>(mbgl::util::RunLoop::Type::New)),
    threadPool(4) {

    mbgl::Log::Info(mbgl::Event::Android, "NativeMapView::NativeMapView");

    //Add a wake function to wake the render thread when needed
    mbgl::util::RunLoop::Impl* loop = reinterpret_cast<mbgl::util::RunLoop::Impl*>(mbgl::util::RunLoop::getLoopHandle());
    loop->setWakeFunction(std::bind(&NativeMapView::wake, this));

    // Get a reference to the JavaVM for callbacks
    //TODO: Why?
    if (_env.GetJavaVM(&vm) < 0) {
        _env.ExceptionDescribe();
        return;
    }

    // Create a default file source for this map instance
    fileSource = std::make_unique<mbgl::DefaultFileSource>(
        jni::Make<std::string>(_env, _cachePath) + "/mbgl-offline.db",
        jni::Make<std::string>(_env, _apkPath));

    // Create the core map
    map = std::make_unique<mbgl::Map>(
        *this, mbgl::Size{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) },
        pixelRatio, *fileSource, threadPool, MapMode::Continuous);

    //Calculate a fitting cache size based on device parameters
    float zoomFactor   = map->getMaxZoom() - map->getMinZoom() + 1;
    float cpuFactor    = availableProcessors;
    float memoryFactor = static_cast<float>(totalMemory) / 1000.0f / 1000.0f / 1000.0f;
    float sizeFactor   = (static_cast<float>(map->getSize().width)  / mbgl::util::tileSize) *
        (static_cast<float>(map->getSize().height) / mbgl::util::tileSize);

    map->setSourceTileCacheSize(zoomFactor * cpuFactor * memoryFactor * sizeFactor * 0.5f);
}

/**
 * Called through NativeMapView#destroy()
 */
NativeMapView::~NativeMapView() {
    mbgl::Log::Info(mbgl::Event::Android, "NativeMapView::~NativeMapView");

    //Remove the wake function as the JVM object is going to be GC'd pretty soon
    mbgl::util::RunLoop::Impl* loop = reinterpret_cast<mbgl::util::RunLoop::Impl*>(mbgl::util::RunLoop::getLoopHandle());
    loop->setWakeFunction(nullptr);

    map.reset();
    fileSource.reset();

    vm = nullptr;
}

/**
 * From mbgl::View
 */
void NativeMapView::bind() {
    getContext().bindFramebuffer = 0;
    getContext().viewport = { 0, 0, getFramebufferSize() };
}

/**
 * From mbgl::Backend. Callback to java NativeMapView#onInvalidate().
 *
 * May be called from any thread
 */
void NativeMapView::invalidate() {
    Log::Info(mbgl::Event::Android, "NativeMapView::invalidate");
    android::UniqueEnv _env = android::AttachEnv();
    static auto onInvalidate = javaClass.GetMethod<void ()>(*_env, "onInvalidate");
    javaPeer->Call(*_env, onInvalidate);
}

/**
 * From mbgl::Backend. Callback to java NativeMapView#onMapChanged(int).
 *
 * May be called from any thread
 */
void NativeMapView::notifyMapChange(mbgl::MapChange change) {
    mbgl::Log::Info(mbgl::Event::Android, "notifyMapChange");
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto onMapChanged = javaClass.GetMethod<void (int)>(*_env, "onMapChanged");
    javaPeer->Call(*_env, onMapChanged, (int) change);
}

// JNI Methods //

void NativeMapView::onViewportChanged(jni::JNIEnv&, jni::jint w, jni::jint h) {
    resizeView((int) w / pixelRatio, (int) h / pixelRatio);
    resizeFramebuffer(w, h);
}

void NativeMapView::render(jni::JNIEnv& env) {
    mbgl::Log::Info(mbgl::Event::Android, "NativeMapView::render");

    if (firstRender) {
        mbgl::Log::Info(mbgl::Event::Android, "Initialize GL extensions");
        mbgl::gl::InitializeExtensions([] (const char * name) {
             return reinterpret_cast<mbgl::gl::glProc>(eglGetProcAddress(name));
        });
        firstRender = false;
    }

    //First, spin the run loop to process the queue (as there is no actual loop on the render thread)
    mbgl::util::RunLoop::Get()->runOnce();

    if (framebufferSizeChanged) {
        getContext().viewport = { 0, 0, getFramebufferSize() };
        framebufferSizeChanged = false;
    }

    updateViewBinding();
    map->render(*this);

    if (snapshot){
        snapshot = false;

        // take snapshot
        auto image = getContext().readFramebuffer<mbgl::PremultipliedImage>(getFramebufferSize());
        auto bitmap = Bitmap::CreateBitmap(env, std::move(image));

        android::UniqueEnv _env = android::AttachEnv();
        static auto onSnapshotReady = javaClass.GetMethod<void (jni::Object<Bitmap>)>(*_env, "onSnapshotReady");
        javaPeer->Call(*_env, onSnapshotReady, bitmap);
    }

    updateFps();
}

void NativeMapView::setAPIBaseUrl(jni::JNIEnv& env, jni::String url) {
    fileSource->setAPIBaseURL(jni::Make<std::string>(env, url));
}

jni::String NativeMapView::getStyleUrl(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, map->getStyleURL());
}

void NativeMapView::setStyleUrl(jni::JNIEnv& env, jni::String url) {
    map->setStyleURL(jni::Make<std::string>(env, url));
}

jni::String NativeMapView::getStyleJson(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, map->getStyleJSON());
}

void NativeMapView::setStyleJson(jni::JNIEnv& env, jni::String json) {
    map->setStyleJSON(jni::Make<std::string>(env, json));
}

jni::String NativeMapView::getAccessToken(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, fileSource->getAccessToken());
}

void NativeMapView::setAccessToken(jni::JNIEnv& env, jni::String token) {
    fileSource->setAccessToken(jni::Make<std::string>(env, token));
}

void NativeMapView::cancelTransitions(jni::JNIEnv&) {
    map->cancelTransitions();
}

void NativeMapView::setGestureInProgress(jni::JNIEnv&, jni::jboolean inProgress) {
    map->setGestureInProgress(inProgress);
}

void NativeMapView::moveBy(jni::JNIEnv&, jni::jdouble dx, jni::jdouble dy) {
    map->moveBy({dx, dy});
}

jni::Object<LatLng> NativeMapView::getLatLng(JNIEnv& env) {
    mbgl::LatLng latLng = map->getLatLng(insets);
    return LatLng::New(env, latLng.latitude, latLng.longitude);
}

void NativeMapView::setLatLng(jni::JNIEnv&, jni::jdouble latitude, jni::jdouble longitude) {
    map->setLatLng(mbgl::LatLng(latitude, longitude), insets);
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

void NativeMapView::setPitch(jni::JNIEnv&, jni::jdouble pitch) {
    map->setPitch(pitch);
}

void NativeMapView::scaleBy(jni::JNIEnv&, jni::jdouble ds, jni::jdouble cx, jni::jdouble cy) {
    mbgl::ScreenCoordinate center(cx, cy);
    map->scaleBy(ds, center);
}

void NativeMapView::setScale(jni::JNIEnv&, jni::jdouble scale, jni::jdouble cx, jni::jdouble cy) {
    mbgl::ScreenCoordinate center(cx, cy);
    map->setScale(scale, center);
}

jni::jdouble NativeMapView::getScale(jni::JNIEnv&) {
    return map->getScale();
}

void NativeMapView::setZoom(jni::JNIEnv&, jni::jdouble zoom) {
    map->setZoom(zoom);
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

void NativeMapView::rotateBy(jni::JNIEnv&, jni::jdouble sx, jni::jdouble sy, jni::jdouble ex, jni::jdouble ey) {
    mbgl::ScreenCoordinate first(sx, sy);
    mbgl::ScreenCoordinate second(ex, ey);
    map->rotateBy(first, second);
}

void NativeMapView::setBearing(jni::JNIEnv&, jni::jdouble degrees) {
    map->setBearing(degrees);
}

void NativeMapView::setBearingXY(jni::JNIEnv&, jni::jdouble degrees, jni::jdouble cx, jni::jdouble cy) {
    mbgl::ScreenCoordinate center(cx, cy);
    map->setBearing(degrees, center);
}

jni::jdouble NativeMapView::getBearing(jni::JNIEnv&) {
    return map->getBearing();
}

void NativeMapView::resetNorth(jni::JNIEnv&) {
    map->resetNorth();
}

void NativeMapView::setVisibleCoordinateBounds(JNIEnv& env, jni::Array<jni::Object<LatLng>> coordinates, jni::Object<RectF> padding, jdouble direction) {
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
    map->easeTo(cameraOptions, animationOptions);
}

void NativeMapView::setContentPadding(JNIEnv&, double top, double left, double bottom, double right) {
    insets = {top, left, bottom, right};
}

void NativeMapView::scheduleSnapshot(jni::JNIEnv&) {
    snapshot = true;
}

void NativeMapView::enableFps(jni::JNIEnv&, jni::jboolean enable) {
    fpsEnabled = enable;
}

jni::Array<jni::jdouble> NativeMapView::getCameraValues(jni::JNIEnv& env) {
    //Create buffer with values
    jdouble buf[5];
    mbgl::LatLng latLng = map->getLatLng(insets);
    buf[0] = latLng.latitude;
    buf[1] = latLng.longitude;
    buf[2] = -map->getBearing();
    buf[3] = map->getPitch();
    buf[4] = map->getZoom();

    //Convert to Java array
    auto output = jni::Array<jni::jdouble>::New(env, 5);
    jni::SetArrayRegion(env, *output, 0, 5, buf);

    return output;
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
    map->onLowMemory();
}

void NativeMapView::setDebug(JNIEnv&, jni::jboolean debug) {
    DebugOptions debugOptions = debug ? DebugOptions::TileBorders | DebugOptions::ParseStatus | DebugOptions::Collision
                                      : DebugOptions::NoDebug;
    map->setDebug(debugOptions);
    fpsEnabled = debug;
}

void NativeMapView::cycleDebugOptions(JNIEnv&) {
    map->cycleDebugOptions();
    fpsEnabled = map->getDebug() != DebugOptions::NoDebug;
}

jni::jboolean NativeMapView::getDebug(JNIEnv&) {
    return map->getDebug() != DebugOptions::NoDebug;
}

jni::jboolean NativeMapView::isFullyLoaded(JNIEnv&) {
    return map->isFullyLoaded();
}

jni::jdouble NativeMapView::getMetersPerPixelAtLatitude(JNIEnv&, jni::jdouble lat, jni::jdouble zoom) {
    return map->getMetersPerPixelAtLatitude(lat, zoom);
}

jni::Object<ProjectedMeters> NativeMapView::projectedMetersForLatLng(JNIEnv& env, jni::jdouble latitude, jni::jdouble longitude) {
    mbgl::ProjectedMeters projectedMeters = map->projectedMetersForLatLng(mbgl::LatLng(latitude, longitude));
    return ProjectedMeters::New(env, projectedMeters.northing, projectedMeters.easting);
}

jni::Object<PointF> NativeMapView::pixelForLatLng(JNIEnv& env, jdouble latitude, jdouble longitude) {
    mbgl::ScreenCoordinate pixel = map->pixelForLatLng(mbgl::LatLng(latitude, longitude));
    return PointF::New(env, static_cast<float>(pixel.x), static_cast<float>(pixel.y));
}

jni::Object<LatLng> NativeMapView::latLngForProjectedMeters(JNIEnv& env, jdouble northing, jdouble easting) {
    mbgl::LatLng latLng = map->latLngForProjectedMeters(mbgl::ProjectedMeters(northing, easting));
    return LatLng::New(env, latLng.latitude, latLng.longitude);
}

jni::Object<LatLng> NativeMapView::latLngForPixel(JNIEnv& env, jfloat x, jfloat y) {
    mbgl::LatLng latLng = map->latLngForPixel(mbgl::ScreenCoordinate(x, y));
    return LatLng::New(env, latLng.latitude, latLng.longitude);
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
    auto iconImage = std::make_shared<mbgl::SpriteImage>(std::move(premultipliedImage), float(scale));
    map->addAnnotationIcon(symbolName, iconImage);
}

jdouble NativeMapView::getTopOffsetPixelsForAnnotationSymbol(JNIEnv& env, jni::String symbolName) {
    return map->getTopOffsetPixelsForAnnotationIcon(jni::Make<std::string>(env, symbolName));
}

jlong NativeMapView::getTransitionDuration(JNIEnv&) {
    const auto transitionOptions = map->getTransitionOptions();
    return transitionOptions.duration.value_or(mbgl::Duration::zero()).count();
}

void NativeMapView::setTransitionDuration(JNIEnv&, jlong duration) {
    auto transitionOptions = map->getTransitionOptions();
    transitionOptions.duration = std::chrono::duration_cast<mbgl::Duration>(std::chrono::duration<jlong>(duration));
    map->setTransitionOptions(transitionOptions);
}

jlong NativeMapView::getTransitionDelay(JNIEnv&) {
    const auto transitionOptions = map->getTransitionOptions();
    return transitionOptions.delay.value_or(mbgl::Duration::zero()).count();
}

void NativeMapView::setTransitionDelay(JNIEnv&, jlong delay) {
    auto transitionOptions = map->getTransitionOptions();
    transitionOptions.delay = std::chrono::duration_cast<mbgl::Duration>(std::chrono::duration<jlong>(delay));
    map->setTransitionOptions(transitionOptions);
}

jni::Array<jlong> NativeMapView::queryPointAnnotations(JNIEnv& env, jni::Object<RectF> rect) {
    // Convert input
    mbgl::ScreenBox box = {
        { RectF::getLeft(env, rect), RectF::getTop(env, rect) },
        { RectF::getRight(env, rect), RectF::getBottom(env, rect) },
    };

    // Assume only points for now
    mbgl::AnnotationIDs ids = map->queryPointAnnotations(box);

    // Convert result
    std::vector<jlong> longIds(ids.begin(), ids.end());
    auto result = jni::Array<jni::jlong>::New(env, ids.size());
    result.SetRegion<std::vector<jni::jlong>>(env, 0, longIds);

    return result;
}

jni::Array<jni::Object<Feature>> NativeMapView::queryRenderedFeaturesForPoint(JNIEnv& env, jni::jfloat x, jni::jfloat y, jni::Array<jni::String> layerIds) {
    using namespace mbgl::android::conversion;
    using namespace mapbox::geometry;

    mbgl::optional<std::vector<std::string>> layers;
    if (layerIds != nullptr && layerIds.Length(env) > 0) {
        layers = toVector(env, layerIds);
    }
    point<double> point = {x, y};

    return *convert<jni::Array<jni::Object<Feature>>, std::vector<mbgl::Feature>>(env, map->queryRenderedFeatures(point, layers));
}

jni::Array<jni::Object<Feature>> NativeMapView::queryRenderedFeaturesForBox(JNIEnv& env, jni::jfloat left, jni::jfloat top, jni::jfloat right, jni::jfloat bottom, jni::Array<jni::String> layerIds) {
    using namespace mbgl::android::conversion;
    using namespace mapbox::geometry;

    mbgl::optional<std::vector<std::string>> layers;
    if (layerIds != nullptr && layerIds.Length(env) > 0) {
        layers = toVector(env, layerIds);
    }
    box<double> box = { point<double>{ left, top}, point<double>{ right, bottom } };

    return *convert<jni::Array<jni::Object<Feature>>, std::vector<mbgl::Feature>>(env, map->queryRenderedFeatures(box, layers));
}

jni::Object<Layer> NativeMapView::getLayer(JNIEnv& env, jni::String layerId) {

    // Find the layer
    mbgl::style::Layer* coreLayer = map->getLayer(jni::Make<std::string>(env, layerId));
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

/**
 * Remove by layer id. Ownership is not transferred back
 */
void NativeMapView::removeLayerById(JNIEnv& env, jni::String id) {
    try {
        map->removeLayer(jni::Make<std::string>(env, id));
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/NoSuchLayerException"), error.what());
    }
}

/**
 * Remove with wrapper object id. Ownership is transferred back to the wrapper
 */
void NativeMapView::removeLayer(JNIEnv& env, jlong layerPtr) {
    assert(layerPtr != 0);

    mbgl::android::Layer *layer = reinterpret_cast<mbgl::android::Layer *>(layerPtr);
    try {
        std::unique_ptr<mbgl::style::Layer> coreLayer = map->removeLayer(layer->get().getID());
        layer->setLayer(std::move(coreLayer));
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/NoSuchLayerException"), error.what());
    }
}


jni::Object<Source> NativeMapView::getSource(JNIEnv& env, jni::String sourceId) {
    // Find the source
    mbgl::style::Source* coreSource = map->getSource(jni::Make<std::string>(env, sourceId));
    if (!coreSource) {
       mbgl::Log::Debug(mbgl::Event::JNI, "No source found");
       return jni::Object<Source>();
    }

    // Create and return the source's native peer
    return jni::Object<Source>(createJavaSourcePeer(env, *map, *coreSource));
}

void NativeMapView::addSource(JNIEnv& env, jni::jlong sourcePtr) {
    assert(sourcePtr != 0);

    Source *source = reinterpret_cast<Source *>(sourcePtr);
    try {
        source->addToMap(*map);
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/sources/CannotAddSourceException"), error.what());
    }
}

void NativeMapView::removeSourceById(JNIEnv& env, jni::String id) {
     try {
         map->removeSource(jni::Make<std::string>(env, id));
     } catch (const std::runtime_error& error) {
         jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/sources/NoSuchSourceException"), error.what());
     }
}

void NativeMapView::removeSource(JNIEnv& env, jlong sourcePtr) {
    assert(sourcePtr != 0);

    mbgl::android::Source *source = reinterpret_cast<mbgl::android::Source *>(sourcePtr);
    try {
        std::unique_ptr<mbgl::style::Source> coreSource = map->removeSource(source->get().getID());
        source->setSource(std::move(coreSource));
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/sources/NoSuchSourceException"), error.what());
    }
}

void NativeMapView::addImage(JNIEnv& env, jni::String name, jni::jint w, jni::jint h, jni::jfloat scale, jni::Array<jbyte> pixels) {
    jni::NullCheck(env, &pixels);
    std::size_t size = pixels.Length(env);

    mbgl::PremultipliedImage premultipliedImage({ static_cast<uint32_t>(w), static_cast<uint32_t>(h) });
    if (premultipliedImage.bytes() != uint32_t(size)) {
        throw mbgl::util::SpriteImageException("Sprite image pixel count mismatch");
    }

    jni::GetArrayRegion(env, *pixels, 0, size, reinterpret_cast<jbyte*>(premultipliedImage.data.get()));
    auto spriteImage = std::make_unique<mbgl::SpriteImage>(std::move(premultipliedImage), float(scale));

    map->addImage(jni::Make<std::string>(env, name), std::move(spriteImage));
}

void NativeMapView::removeImage(JNIEnv& env, jni::String name) {
    map->removeImage(jni::Make<std::string>(env, name));
}

// Private methods //

mbgl::Size NativeMapView::getFramebufferSize() const {
    mbgl::Log::Info(mbgl::Event::Android, "FB size %dx%d", fbWidth, fbHeight);
    return { static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight) };
}

/**
 * Called whenever the associated thread needs to wake up (since there is no active run loop)
 *
 * May be called from any thread
 */
void NativeMapView::wake() {
    mbgl::Log::Info(mbgl::Event::JNI, "Wake callback");
    android::UniqueEnv _env = android::AttachEnv();
    static auto wakeCallback = javaClass.GetMethod<void ()>(*_env, "onWake");
    javaPeer->Call(*_env, wakeCallback);
}

void NativeMapView::updateViewBinding() {
    getContext().bindFramebuffer.setCurrentValue(0);
    assert(mbgl::gl::value::BindFramebuffer::Get() == getContext().bindFramebuffer.getCurrentValue());
    getContext().viewport.setCurrentValue({ 0, 0, getFramebufferSize() });
    assert(mbgl::gl::value::Viewport::Get() == getContext().viewport.getCurrentValue());
}

void NativeMapView::resizeView(int w, int h) {
    mbgl::Log::Info(mbgl::Event::Android, "resizeView %ix%i", w, h);
    width = w;
    height = h;
    map->setSize({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
}

void NativeMapView::resizeFramebuffer(int w, int h) {
    mbgl::Log::Info(mbgl::Event::Android, "resizeFramebuffer %ix%i", w, h);
    fbWidth = w;
    fbHeight = h;
    framebufferSizeChanged = true;
    invalidate();
}

void NativeMapView::updateFps() {
    if (!fpsEnabled) {
        return;
    }

    static int frames = 0;
    static int64_t timeElapsed = 0LL;

    frames++;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    int64_t currentTime = now.tv_sec * 1000000000LL + now.tv_nsec;

    if (currentTime - timeElapsed >= 1) {
        fps = frames / ((currentTime - timeElapsed) / 1E9);
        mbgl::Log::Info(mbgl::Event::Render, "FPS: %4.2f", fps);
        timeElapsed = currentTime;
        frames = 0;
    }

    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto onFpsChanged = javaClass.GetMethod<void (double)>(*_env, "onFpsChanged");
    javaPeer->Call(*_env, onFpsChanged, fps);
}

// Static methods //

jni::Class<NativeMapView> NativeMapView::javaClass;

void NativeMapView::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    NativeMapView::javaClass = *jni::Class<NativeMapView>::Find(env).NewGlobalRef(env).release();

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<NativeMapView>(env, NativeMapView::javaClass, "nativePtr",
            std::make_unique<NativeMapView, JNIEnv&, jni::Object<NativeMapView>, jni::String, jni::String, jni::jfloat, jni::jint, jni::jlong>,
            "initialize",
            "destroy",
            METHOD(&NativeMapView::render, "render"),
            METHOD(&NativeMapView::onViewportChanged, "_onViewportChanged"),
            METHOD(&NativeMapView::setAPIBaseUrl  , "setApiBaseUrl"),
            METHOD(&NativeMapView::getStyleUrl, "getStyleUrl"),
            METHOD(&NativeMapView::setStyleUrl, "setStyleUrl"),
            METHOD(&NativeMapView::getStyleJson, "getStyleJson"),
            METHOD(&NativeMapView::setStyleJson, "setStyleJson"),
            METHOD(&NativeMapView::getAccessToken, "getAccessToken"),
            METHOD(&NativeMapView::setAccessToken, "setAccessToken"),
            METHOD(&NativeMapView::cancelTransitions, "cancelTransitions"),
            METHOD(&NativeMapView::setGestureInProgress, "setGestureInProgress"),
            METHOD(&NativeMapView::moveBy, "_moveBy"),
            METHOD(&NativeMapView::getLatLng, "_getLatLng"),
            METHOD(&NativeMapView::setLatLng, "setLatLng"),
            METHOD(&NativeMapView::setReachability, "setReachability"),
            METHOD(&NativeMapView::resetPosition, "resetPosition"),
            METHOD(&NativeMapView::getPitch, "getPitch"),
            METHOD(&NativeMapView::setPitch, "setPitch"),
            METHOD(&NativeMapView::scaleBy, "_scaleBy"),
            METHOD(&NativeMapView::setScale, "_setScale"),
            METHOD(&NativeMapView::getScale, "getScale"),
            METHOD(&NativeMapView::setZoom, "setZoom"),
            METHOD(&NativeMapView::getZoom, "getZoom"),
            METHOD(&NativeMapView::resetZoom, "resetZoom"),
            METHOD(&NativeMapView::setMinZoom, "setMinZoom"),
            METHOD(&NativeMapView::getMinZoom, "getMinZoom"),
            METHOD(&NativeMapView::setMaxZoom, "setMaxZoom"),
            METHOD(&NativeMapView::getMaxZoom, "getMaxZoom"),
            METHOD(&NativeMapView::rotateBy, "_rotateBy"),
            METHOD(&NativeMapView::setBearing, "setBearing"),
            METHOD(&NativeMapView::setBearingXY, "_setBearingXY"),
            METHOD(&NativeMapView::getBearing, "getBearing"),
            METHOD(&NativeMapView::resetNorth, "resetNorth"),
            METHOD(&NativeMapView::setVisibleCoordinateBounds, "setVisibleCoordinateBounds"),
            METHOD(&NativeMapView::setContentPadding, "setContentPadding"),
            METHOD(&NativeMapView::scheduleSnapshot, "scheduleSnapshot"),
            METHOD(&NativeMapView::enableFps, "enableFps"),
            METHOD(&NativeMapView::getCameraValues, "getCameraValues"),
            METHOD(&NativeMapView::updateMarker, "updateMarker"),
            METHOD(&NativeMapView::addMarkers, "addMarkers"),
            METHOD(&NativeMapView::setDebug, "setDebug"),
            METHOD(&NativeMapView::cycleDebugOptions, "cycleDebugOptions"),
            METHOD(&NativeMapView::getDebug, "getDebug"),
            METHOD(&NativeMapView::isFullyLoaded, "isFullyLoaded"),
            METHOD(&NativeMapView::onLowMemory, "onLowMemory"),
            METHOD(&NativeMapView::getMetersPerPixelAtLatitude, "getMetersPerPixelAtLatitude"),
            METHOD(&NativeMapView::projectedMetersForLatLng, "projectedMetersForLatLng"),
            METHOD(&NativeMapView::pixelForLatLng, "pixelForLatLng"),
            METHOD(&NativeMapView::latLngForProjectedMeters, "latLngForProjectedMeters"),
            METHOD(&NativeMapView::latLngForPixel, "latLngForPixel"),
            METHOD(&NativeMapView::addPolylines, "addPolylines"),
            METHOD(&NativeMapView::addPolygons, "addPolygons"),
            METHOD(&NativeMapView::updatePolyline, "updatePolyline"),
            METHOD(&NativeMapView::updatePolygon, "updatePolygon"),
            METHOD(&NativeMapView::removeAnnotations, "removeAnnotations"),
            METHOD(&NativeMapView::addAnnotationIcon, "addAnnotationIcon"),
            METHOD(&NativeMapView::getTopOffsetPixelsForAnnotationSymbol, "getTopOffsetPixelsForAnnotationSymbol"),
            METHOD(&NativeMapView::getTransitionDuration, "getTransitionDuration"),
            METHOD(&NativeMapView::setTransitionDuration, "setTransitionDuration"),
            METHOD(&NativeMapView::getTransitionDelay, "getTransitionDelay"),
            METHOD(&NativeMapView::setTransitionDelay, "setTransitionDelay"),
            METHOD(&NativeMapView::queryPointAnnotations, "queryPointAnnotations"),
            METHOD(&NativeMapView::queryRenderedFeaturesForPoint, "queryRenderedFeaturesForPoint"),
            METHOD(&NativeMapView::queryRenderedFeaturesForBox, "queryRenderedFeaturesForBox"),
            METHOD(&NativeMapView::getLayer, "getLayer"),
            METHOD(&NativeMapView::addLayer, "addLayer"),
            METHOD(&NativeMapView::removeLayerById, "removeLayerById"),
            METHOD(&NativeMapView::removeLayer, "removeLayer"),
            METHOD(&NativeMapView::getSource, "getSource"),
            METHOD(&NativeMapView::addSource, "addSource"),
            METHOD(&NativeMapView::removeSourceById, "removeSourceById"),
            METHOD(&NativeMapView::removeSource, "removeSource"),
            METHOD(&NativeMapView::addImage, "addImage"),
            METHOD(&NativeMapView::removeImage, "removeImage")
    );
}

}
}
