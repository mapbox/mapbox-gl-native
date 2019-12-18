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

#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/event.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/renderer/query.hpp>

// Java -> C++ conversion
#include "style/android_conversion.hpp"
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>

// C++ -> Java conversion
#include "conversion/conversion.hpp"
#include "conversion/collection.hpp"
#include "style/conversion/filter.hpp"
#include "geojson/feature.hpp"

#include "android_renderer_frontend.hpp"
#include "attach_env.hpp"
#include "bitmap.hpp"
#include "bitmap_factory.hpp"
#include "file_source.hpp"
#include "geometry/lat_lng_bounds.hpp"
#include "java/util.hpp"
#include "jni.hpp"
#include "map/camera_position.hpp"
#include "map/image.hpp"
#include "map_renderer.hpp"
#include "run_loop_impl.hpp"
#include "style/light.hpp"

namespace mbgl {
namespace android {

NativeMapView::NativeMapView(jni::JNIEnv& _env,
                             const jni::Object<NativeMapView>& _obj,
                             const jni::Object<FileSource>& jFileSource,
                             const jni::Object<MapRenderer>& jMapRenderer,
                             jni::jfloat _pixelRatio,
                             jni::jboolean _crossSourceCollisions)
    : javaPeer(_env, _obj)
    , mapRenderer(MapRenderer::getNativePeer(_env, jMapRenderer))
    , pixelRatio(_pixelRatio) {

    // Get a reference to the JavaVM for callbacks
    if (_env.GetJavaVM(&vm) < 0) {
        _env.ExceptionDescribe();
        return;
    }

    // Create a renderer frontend
    rendererFrontend = std::make_unique<AndroidRendererFrontend>(mapRenderer);

    // Create Map options
    MapOptions options;
    options.withMapMode(MapMode::Continuous)
           .withSize(mbgl::Size{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) })
           .withPixelRatio(pixelRatio)
           .withConstrainMode(ConstrainMode::HeightOnly)
           .withViewportMode(ViewportMode::Default)
           .withCrossSourceCollisions(_crossSourceCollisions);

    // Create the core map
    map = std::make_unique<mbgl::Map>(
        *rendererFrontend, *this, options,
        mbgl::android::FileSource::getSharedResourceOptions(_env, jFileSource));
}

/**
 * Called through NativeMapView#destroy()
 */
NativeMapView::~NativeMapView() {
    map.reset();
    vm = nullptr;
}

void NativeMapView::onCameraWillChange(MapObserver::CameraChangeMode mode) {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onCameraWillChange = javaClass.GetMethod<void (jboolean)>(*_env, "onCameraWillChange");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onCameraWillChange, (jboolean) (mode != MapObserver::CameraChangeMode::Immediate));
    }
}


void NativeMapView::onCameraIsChanging() {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onCameraIsChanging = javaClass.GetMethod<void ()>(*_env, "onCameraIsChanging");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onCameraIsChanging);
    }
}

void NativeMapView::onCameraDidChange(MapObserver::CameraChangeMode mode) {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onCameraDidChange = javaClass.GetMethod<void (jboolean)>(*_env, "onCameraDidChange");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onCameraDidChange,
                                 (jboolean) (mode != MapObserver::CameraChangeMode::Immediate));
    }
}

void NativeMapView::onWillStartLoadingMap() {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onWillStartLoadingMap = javaClass.GetMethod<void ()>(*_env, "onWillStartLoadingMap");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onWillStartLoadingMap);
    }
}

void NativeMapView::onDidFinishLoadingMap() {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onDidFinishLoadingMap = javaClass.GetMethod<void ()>(*_env, "onDidFinishLoadingMap");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onDidFinishLoadingMap);
    }
}

void NativeMapView::onDidFailLoadingMap(MapLoadError, const std::string& error) {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onDidFailLoadingMap = javaClass.GetMethod<void (jni::String)>(*_env, "onDidFailLoadingMap");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onDidFailLoadingMap, jni::Make<jni::String>(*_env, error));
    }
}

void NativeMapView::onWillStartRenderingFrame() {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onWillStartRenderingFrame = javaClass.GetMethod<void ()>(*_env, "onWillStartRenderingFrame");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onWillStartRenderingFrame);
    }
}

void NativeMapView::onDidFinishRenderingFrame(MapObserver::RenderFrameStatus status) {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onDidFinishRenderingFrame = javaClass.GetMethod<void (jboolean)>(*_env, "onDidFinishRenderingFrame");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onDidFinishRenderingFrame, (jboolean) (status.mode != MapObserver::RenderMode::Partial));
    }
}


void NativeMapView::onWillStartRenderingMap() {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onWillStartRenderingMap = javaClass.GetMethod<void ()>(*_env, "onWillStartRenderingMap");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onWillStartRenderingMap);
    }
}

void NativeMapView::onDidFinishRenderingMap(MapObserver::RenderMode mode) {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onDidFinishRenderingMap = javaClass.GetMethod<void (jboolean)>(*_env, "onDidFinishRenderingMap");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onDidFinishRenderingMap, (jboolean) (mode != MapObserver::RenderMode::Partial));
    }
}

void NativeMapView::onDidBecomeIdle() {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onDidBecomeIdle = javaClass.GetMethod<void ()>(*_env, "onDidBecomeIdle");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onDidBecomeIdle);
    }
}

void NativeMapView::onDidFinishLoadingStyle() {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onDidFinishLoadingStyle = javaClass.GetMethod<void ()>(*_env, "onDidFinishLoadingStyle");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onDidFinishLoadingStyle);
    }
}

void NativeMapView::onSourceChanged(mbgl::style::Source& source) {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onSourceChanged = javaClass.GetMethod<void (jni::String)>(*_env, "onSourceChanged");
    auto sourceId = jni::Make<jni::String>(*_env, source.getID());
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onSourceChanged, sourceId);
    }
}

void NativeMapView::onStyleImageMissing(const std::string& imageId) {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onStyleImageMissing = javaClass.GetMethod<void (jni::String)>(*_env, "onStyleImageMissing");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        weakReference.Call(*_env, onStyleImageMissing, jni::Make<jni::String>(*_env, imageId));
    }
}

bool NativeMapView::onCanRemoveUnusedStyleImage(const std::string& imageId) {
    assert(vm != nullptr);

    android::UniqueEnv _env = android::AttachEnv();
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
    static auto onCanRemoveUnusedStyleImage = javaClass.GetMethod<jboolean (jni::String)>(*_env, "onCanRemoveUnusedStyleImage");
    auto weakReference = javaPeer.get(*_env);
    if (weakReference) {
        return weakReference.Call(*_env, onCanRemoveUnusedStyleImage, jni::Make<jni::String>(*_env, imageId));
    }

    return true;
}

// JNI Methods //

void NativeMapView::resizeView(jni::JNIEnv&, int w, int h) {
    width = util::max(64, w);
    height = util::max(64, h);
    map->setSize({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
}

jni::Local<jni::String> NativeMapView::getStyleUrl(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, map->getStyle().getURL());
}

void NativeMapView::setStyleUrl(jni::JNIEnv& env, const jni::String& url) {
    map->getStyle().loadURL(jni::Make<std::string>(env, url));
}

jni::Local<jni::String> NativeMapView::getStyleJson(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, map->getStyle().getJSON());
}

void NativeMapView::setStyleJson(jni::JNIEnv& env, const jni::String& json) {
    map->getStyle().loadJSON(jni::Make<std::string>(env, json));
}

void NativeMapView::setLatLngBounds(jni::JNIEnv& env, const jni::Object<mbgl::android::LatLngBounds>& jBounds) {
    mbgl::BoundOptions bounds;
    if (jBounds) {
        bounds.withLatLngBounds(mbgl::android::LatLngBounds::getLatLngBounds(env, jBounds));
    } else {
        bounds.withLatLngBounds(mbgl::LatLngBounds());
    }
    map->setBounds(bounds);
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

void NativeMapView::jumpTo(jni::JNIEnv& env, jni::jdouble bearing, jni::jdouble latitude, jni::jdouble longitude, jni::jdouble pitch, jni::jdouble zoom, const jni::Array<jni::jdouble>& padding) {
    mbgl::CameraOptions options;
    if (bearing != -1) {
        options.bearing = bearing;
    }
    options.center = mbgl::LatLng(latitude, longitude);
    if (padding) {
        assert(padding.Length(env) == 4);
        options.padding = mbgl::EdgeInsets{padding.Get(env, 0), padding.Get(env, 1),
                                           padding.Get(env, 2), padding.Get(env, 3)};
    }
    if (pitch != -1) {
        options.pitch = pitch;
    }
    if (zoom != -1) {
        options.zoom = zoom;
    }

    map->jumpTo(options);
}

void NativeMapView::easeTo(jni::JNIEnv& env, jni::jdouble bearing, jni::jdouble latitude, jni::jdouble longitude, jni::jlong duration, jni::jdouble pitch, jni::jdouble zoom, const jni::Array<jni::jdouble>& padding, jni::jboolean easing) {
    mbgl::CameraOptions cameraOptions;
    if (bearing != -1) {
        cameraOptions.bearing = bearing;
    }
    cameraOptions.center = mbgl::LatLng(latitude, longitude);
    if (padding) {
        assert(padding.Length(env) == 4);
        cameraOptions.padding = mbgl::EdgeInsets{padding.Get(env, 0), padding.Get(env, 1),
                                           padding.Get(env, 2), padding.Get(env, 3)};
    }
    if (pitch != -1) {
        cameraOptions.pitch = pitch;
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

void NativeMapView::flyTo(jni::JNIEnv& env, jni::jdouble bearing, jni::jdouble latitude, jni::jdouble longitude, jni::jlong duration, jni::jdouble pitch, jni::jdouble zoom, const jni::Array<jni::jdouble>& padding) {
    mbgl::CameraOptions cameraOptions;
    if (bearing != -1) {
        cameraOptions.bearing = bearing;
    }
    cameraOptions.center = mbgl::LatLng(latitude, longitude);
    if (padding) {
        assert(padding.Length(env) == 4);
        cameraOptions.padding = mbgl::EdgeInsets{padding.Get(env, 0), padding.Get(env, 1),
                                                 padding.Get(env, 2), padding.Get(env, 3)};
    }
    if (pitch != -1) {
        cameraOptions.pitch = pitch;
    }
    if (zoom != -1) {
        cameraOptions.zoom = zoom;
    }

    mbgl::AnimationOptions animationOptions;
    animationOptions.duration.emplace(mbgl::Milliseconds(duration));
    map->flyTo(cameraOptions, animationOptions);
}

jni::Local<jni::Object<LatLng>> NativeMapView::getLatLng(JNIEnv& env) {
    return LatLng::New(env, *map->getCameraOptions(mbgl::nullopt).center);
}

void NativeMapView::setLatLng(jni::JNIEnv& env, jni::jdouble latitude, jni::jdouble longitude, const jni::Array<jni::jdouble>& padding, jni::jlong duration) {
    mbgl::CameraOptions cameraOptions;
    cameraOptions.center = mbgl::LatLng(latitude, longitude);
    if (padding) {
        assert(padding.Length(env) == 4);
        cameraOptions.padding = mbgl::EdgeInsets{padding.Get(env, 0), padding.Get(env, 1),
                                                 padding.Get(env, 2), padding.Get(env, 3)};
    }
    map->easeTo(cameraOptions, mbgl::AnimationOptions{mbgl::Milliseconds(duration)});
}

jni::Local<jni::Object<CameraPosition>> NativeMapView::getCameraForLatLngBounds(jni::JNIEnv& env, const jni::Object<LatLngBounds>& jBounds, double top, double left, double bottom, double right, double bearing, double tilt) {
    mbgl::EdgeInsets padding = {top, left, bottom, right};
    return CameraPosition::New(env, map->cameraForLatLngBounds(mbgl::android::LatLngBounds::getLatLngBounds(env, jBounds), padding, bearing, tilt), pixelRatio);
}

jni::Local<jni::Object<CameraPosition>> NativeMapView::getCameraForGeometry(jni::JNIEnv& env, const jni::Object<geojson::Geometry>& jGeometry, double top, double left, double bottom, double right, double bearing, double tilt) {
    auto geometry = geojson::Geometry::convert(env, jGeometry);
    mbgl::EdgeInsets padding = {top, left, bottom, right};
    return CameraPosition::New(env, map->cameraForGeometry(geometry, padding, bearing, tilt), pixelRatio);
}

void NativeMapView::setReachability(jni::JNIEnv&, jni::jboolean reachable) {
    if (reachable) {
        mbgl::NetworkStatus::Reachable();
    }
}

void NativeMapView::resetPosition(jni::JNIEnv&) {
    map->jumpTo(mbgl::CameraOptions().withCenter(mbgl::LatLng {}).withZoom(0.0).withBearing(0.0).withPitch(0.0));
}

jni::jdouble NativeMapView::getPitch(jni::JNIEnv&) {
    return *map->getCameraOptions().pitch;
}

void NativeMapView::setPitch(jni::JNIEnv&, jni::jdouble pitch, jni::jlong duration) {
    map->easeTo(mbgl::CameraOptions().withPitch(pitch),
                mbgl::AnimationOptions{ mbgl::Milliseconds(duration) });
}

void NativeMapView::setZoom(jni::JNIEnv&, jni::jdouble zoom, jni::jdouble x, jni::jdouble y, jni::jlong duration) {
    map->easeTo(mbgl::CameraOptions().withZoom(zoom).withAnchor(mbgl::ScreenCoordinate{ x, y }),
                mbgl::AnimationOptions{ mbgl::Milliseconds(duration) });
}

jni::jdouble NativeMapView::getZoom(jni::JNIEnv&) {
    return *map->getCameraOptions().zoom;
}

void NativeMapView::resetZoom(jni::JNIEnv&) {
    map->jumpTo(mbgl::CameraOptions().withZoom(0.0));
}

void NativeMapView::setMinZoom(jni::JNIEnv&, jni::jdouble zoom) {
    map->setBounds(BoundOptions().withMinZoom(zoom));
}

jni::jdouble NativeMapView::getMinZoom(jni::JNIEnv&) {
    return *map->getBounds().minZoom;
}

void NativeMapView::setMaxZoom(jni::JNIEnv&, jni::jdouble zoom) {
    map->setBounds(BoundOptions().withMaxZoom(zoom));
}

jni::jdouble NativeMapView::getMaxZoom(jni::JNIEnv&) {
    return *map->getBounds().maxZoom;
}

void NativeMapView::rotateBy(jni::JNIEnv&, jni::jdouble sx, jni::jdouble sy, jni::jdouble ex, jni::jdouble ey, jni::jlong duration) {
    mbgl::ScreenCoordinate first(sx, sy);
    mbgl::ScreenCoordinate second(ex, ey);
    map->rotateBy(first, second, mbgl::AnimationOptions{mbgl::Milliseconds(duration)});
}

void NativeMapView::setBearing(jni::JNIEnv&, jni::jdouble degrees, jni::jlong duration) {
    map->easeTo(mbgl::CameraOptions().withBearing(degrees), mbgl::AnimationOptions{mbgl::Milliseconds(duration)});
}

void NativeMapView::setBearingXY(jni::JNIEnv&, jni::jdouble degrees, jni::jdouble cx, jni::jdouble cy, jni::jlong duration) {
    mbgl::ScreenCoordinate anchor(cx, cy);
    map->easeTo(mbgl::CameraOptions().withBearing(degrees).withAnchor(anchor), mbgl::AnimationOptions{mbgl::Milliseconds(duration)});
}

jni::jdouble NativeMapView::getBearing(jni::JNIEnv&) {
    return *map->getCameraOptions().bearing;
}

void NativeMapView::resetNorth(jni::JNIEnv&) {
    map->easeTo(mbgl::CameraOptions().withBearing(0.0), mbgl::AnimationOptions {{mbgl::Milliseconds(500)}});
}

void NativeMapView::setVisibleCoordinateBounds(JNIEnv& env, const jni::Array<jni::Object<LatLng>>& coordinates, const jni::Object<RectF>& padding, jdouble direction, jni::jlong duration) {
    NullCheck(env, &coordinates);
    std::size_t count = coordinates.Length(env);

    std::vector<mbgl::LatLng> latLngs;
    latLngs.reserve(count);

    for (std::size_t i = 0; i < count; i++) {
        latLngs.push_back(LatLng::getLatLng(env, coordinates.Get(env, i)));
    }

    mbgl::EdgeInsets mbglInsets = { RectF::getTop(env, padding), RectF::getLeft(env, padding), RectF::getBottom(env, padding), RectF::getRight(env, padding) };
    mbgl::CameraOptions cameraOptions = map->cameraForLatLngs(latLngs, mbglInsets);
    if (direction >= 0) {
        cameraOptions.bearing = direction;
    }

    mbgl::AnimationOptions animationOptions;
    if (duration > 0) {
        animationOptions.duration.emplace(mbgl::Milliseconds(duration));
        // equivalent to kCAMediaTimingFunctionDefault in iOS
        animationOptions.easing.emplace(mbgl::util::UnitBezier { 0.25, 0.1, 0.25, 0.1 });
    }

    map->easeTo(cameraOptions, animationOptions);
}

void NativeMapView::scheduleSnapshot(jni::JNIEnv&) {
    mapRenderer.requestSnapshot([&](PremultipliedImage image) {
        auto _env = android::AttachEnv();
        // Convert image to bitmap
        auto bitmap = Bitmap::CreateBitmap(*_env, std::move(image));

        // invoke Mapview#OnSnapshotReady
        static auto& javaClass = jni::Class<NativeMapView>::Singleton(*_env);
        static auto onSnapshotReady = javaClass.GetMethod<void (jni::Object<Bitmap>)>(*_env, "onSnapshotReady");
        auto weakReference = javaPeer.get(*_env);
        if (weakReference) {
            weakReference.Call(*_env, onSnapshotReady, bitmap);
        }
    });
}

jni::Local<jni::Object<CameraPosition>> NativeMapView::getCameraPosition(jni::JNIEnv& env) {
    return CameraPosition::New(env, map->getCameraOptions(mbgl::nullopt), pixelRatio);
}

void NativeMapView::updateMarker(jni::JNIEnv& env, jni::jlong markerId, jni::jdouble lat, jni::jdouble lon, const jni::String& jid) {
    if (markerId == -1) {
        return;
    }

    std::string iconId = jni::Make<std::string>(env, jid);
    // Because Java only has int, not unsigned int, we need to bump the annotation id up to a long.
    map->updateAnnotation(markerId, mbgl::SymbolAnnotation { mbgl::Point<double>(lon, lat), iconId });
}

jni::Local<jni::Array<jni::jlong>> NativeMapView::addMarkers(jni::JNIEnv& env, const jni::Array<jni::Object<Marker>>& jmarkers) {
    jni::NullCheck(env, &jmarkers);
    std::size_t len = jmarkers.Length(env);

    std::vector<jni::jlong> ids;
    ids.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        auto marker = jmarkers.Get(env, i);
        ids.push_back(map->addAnnotation(mbgl::SymbolAnnotation {
            Marker::getPosition(env, marker),
            Marker::getIconId(env, marker)
        }));
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

jni::Local<jni::Object<ProjectedMeters>> NativeMapView::projectedMetersForLatLng(JNIEnv& env, jni::jdouble latitude, jni::jdouble longitude) {
    mbgl::ProjectedMeters projectedMeters = mbgl::Projection::projectedMetersForLatLng(mbgl::LatLng(latitude, longitude));
    return ProjectedMeters::New(env, projectedMeters.northing(), projectedMeters.easting());
}

jni::Local<jni::Object<LatLng>> NativeMapView::latLngForProjectedMeters(JNIEnv& env, jdouble northing, jdouble easting) {
    return LatLng::New(env, mbgl::Projection::latLngForProjectedMeters(mbgl::ProjectedMeters(northing, easting)));
}

jni::Local<jni::Object<PointF>> NativeMapView::pixelForLatLng(JNIEnv& env, jdouble latitude, jdouble longitude) {
    mbgl::ScreenCoordinate pixel = map->pixelForLatLng(mbgl::LatLng(latitude, longitude));
    return PointF::New(env, static_cast<float>(pixel.x), static_cast<float>(pixel.y));
}

void NativeMapView::pixelsForLatLngs(JNIEnv& env,
                                     const jni::Array<jdouble>& input,
                                     jni::Array<jdouble>& output,
                                     jfloat pixelRatio) {
    jni::NullCheck(env, &input);
    std::size_t len = input.Length(env);

    std::vector<mbgl::LatLng> latLngs;
    latLngs.reserve(len);

    for (std::size_t i = 0; i < len; i += 2) {
        auto latLng = mbgl::LatLng(input.Get(env, i), input.Get(env, i + 1));
        latLngs.push_back(latLng);
    }

    std::vector<jdouble> buffer;
    buffer.reserve(len);
    std::vector<ScreenCoordinate> coordinates = map->pixelsForLatLngs(latLngs);
    for (std::size_t i = 0; i < len / 2; i++) {
        buffer.push_back(coordinates[i].x * pixelRatio);
        buffer.push_back(coordinates[i].y * pixelRatio);
    }

    output.SetRegion<std::vector<jdouble>>(env, 0, buffer);
}

jni::Local<jni::Object<LatLng>> NativeMapView::latLngForPixel(JNIEnv& env, jfloat x, jfloat y) {
    return LatLng::New(env, map->latLngForPixel(mbgl::ScreenCoordinate(x, y)));
}

void NativeMapView::latLngsForPixels(JNIEnv& env,
                                     const jni::Array<jdouble>& input,
                                     jni::Array<jdouble>& output,
                                     jfloat pixelRatio) {
    jni::NullCheck(env, &input);
    std::size_t len = input.Length(env);

    std::vector<mbgl::ScreenCoordinate> coordinates;
    coordinates.reserve(len);

    for (std::size_t i = 0; i < len; i += 2) {
        auto coordinate = mbgl::ScreenCoordinate(input.Get(env, i) / pixelRatio, input.Get(env, i + 1) / pixelRatio);
        coordinates.push_back(coordinate);
    }

    std::vector<jdouble> buffer;
    buffer.reserve(len);
    std::vector<mbgl::LatLng> latLngs = map->latLngsForPixels(coordinates);
    for (std::size_t i = 0; i < len / 2; i++) {
        buffer.push_back(latLngs[i].latitude());
        buffer.push_back(latLngs[i].longitude());
    }

    output.SetRegion<std::vector<jdouble>>(env, 0, buffer);
}

jni::Local<jni::Array<jlong>> NativeMapView::addPolylines(JNIEnv& env, const jni::Array<jni::Object<Polyline>>& polylines) {
    NullCheck(env, &polylines);
    std::size_t len = polylines.Length(env);

    std::vector<jni::jlong> ids;
    ids.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        mbgl::LineAnnotation annotation = Polyline::toAnnotation(env, polylines.Get(env, i));
        ids.push_back(map->addAnnotation(annotation));
    }

    auto result = jni::Array<jni::jlong>::New(env, len);
    result.SetRegion<std::vector<jni::jlong>>(env, 0, ids);

    return result;
}


jni::Local<jni::Array<jlong>> NativeMapView::addPolygons(JNIEnv& env, const jni::Array<jni::Object<Polygon>>& polygons) {
    NullCheck(env, &polygons);
    std::size_t len = polygons.Length(env);

    std::vector<jni::jlong> ids;
    ids.reserve(len);

    for (std::size_t i = 0; i < len; i++) {
        mbgl::FillAnnotation annotation = Polygon::toAnnotation(env, polygons.Get(env, i));
        ids.push_back(map->addAnnotation(annotation));
    }

    auto result = jni::Array<jni::jlong>::New(env, len);
    result.SetRegion<std::vector<jni::jlong>>(env, 0, ids);

    return result;
}

void NativeMapView::updatePolyline(JNIEnv& env, jlong polylineId, const jni::Object<Polyline>& polyline) {
    mbgl::LineAnnotation annotation = Polyline::toAnnotation(env, polyline);
    map->updateAnnotation(polylineId, annotation);
}

void NativeMapView::updatePolygon(JNIEnv& env, jlong polygonId, const jni::Object<Polygon>& polygon) {
    mbgl::FillAnnotation annotation = Polygon::toAnnotation(env, polygon);
    map->updateAnnotation(polygonId, annotation);
}

void NativeMapView::removeAnnotations(JNIEnv& env, const jni::Array<jlong>& ids) {
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

void NativeMapView::addAnnotationIcon(JNIEnv& env, const jni::String& symbol, jint w, jint h, jfloat scale, const jni::Array<jbyte>& jpixels) {
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

void NativeMapView::removeAnnotationIcon(JNIEnv& env, const jni::String& symbol) {
    const std::string symbolName = jni::Make<std::string>(env, symbol);
    map->removeAnnotationImage(symbolName);
}

jdouble NativeMapView::getTopOffsetPixelsForAnnotationSymbol(JNIEnv& env, const jni::String& symbolName) {
    return map->getTopOffsetPixelsForAnnotationImage(jni::Make<std::string>(env, symbolName));
}

jni::Local<jni::Object<TransitionOptions>> NativeMapView::getTransitionOptions(JNIEnv& env) {
    const auto transitionOptions = map->getStyle().getTransitionOptions();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(transitionOptions.duration.value_or(mbgl::Duration::zero())).count();
    const auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(transitionOptions.delay.value_or(mbgl::Duration::zero())).count();
    const auto enablePlacementTransitions = (jboolean) transitionOptions.enablePlacementTransitions;
    return TransitionOptions::fromTransitionOptions(env, duration, delay, enablePlacementTransitions);
}

void NativeMapView::setTransitionOptions(JNIEnv& env, const jni::Object<TransitionOptions>& options) {
    const mbgl::style::TransitionOptions transitionOptions(
            Duration(mbgl::Milliseconds(TransitionOptions::getDuration(env, options))),
            Duration(mbgl::Milliseconds(TransitionOptions::getDelay(env, options))),
            TransitionOptions::isEnablePlacementTransitions(env, options)
    );
    map->getStyle().setTransitionOptions(transitionOptions);
}

jni::Local<jni::Array<jlong>> NativeMapView::queryPointAnnotations(JNIEnv& env, const jni::Object<RectF>& rect) {
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

jni::Local<jni::Array<jlong>> NativeMapView::queryShapeAnnotations(JNIEnv& env, const jni::Object<RectF>& rect) {
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

jni::Local<jni::Array<jni::Object<geojson::Feature>>> NativeMapView::queryRenderedFeaturesForPoint(JNIEnv& env, jni::jfloat x, jni::jfloat y,
                                                                              const jni::Array<jni::String>& layerIds,
                                                                              const jni::Array<jni::Object<>>& jfilter) {
    using namespace mbgl::android::conversion;
    using namespace mbgl::android::geojson;

    mbgl::optional<std::vector<std::string>> layers;
    if (layerIds && layerIds.Length(env) > 0) {
        layers = android::conversion::toVector(env, layerIds);
    }
    mapbox::geometry::point<double> point = {x, y};

    return Feature::convert(
            env,
            rendererFrontend->queryRenderedFeatures(point, { layers, toFilter(env, jfilter) }));
}

jni::Local<jni::Array<jni::Object<geojson::Feature>>> NativeMapView::queryRenderedFeaturesForBox(JNIEnv& env, jni::jfloat left, jni::jfloat top,
                                                                            jni::jfloat right, jni::jfloat bottom, const jni::Array<jni::String>& layerIds,
                                                                            const jni::Array<jni::Object<>>& jfilter) {
    using namespace mbgl::android::conversion;
    using namespace mbgl::android::geojson;

    mbgl::optional<std::vector<std::string>> layers;
    if (layerIds && layerIds.Length(env) > 0) {
        layers = toVector(env, layerIds);
    }
    mapbox::geometry::box<double> box = {
            mapbox::geometry::point<double>{ left, top},
            mapbox::geometry::point<double>{ right, bottom }
    };

    return Feature::convert(
            env,
            rendererFrontend->queryRenderedFeatures(box, { layers, toFilter(env, jfilter) }));
}

jni::Local<jni::Object<Light>> NativeMapView::getLight(JNIEnv& env) {
    mbgl::style::Light* light = map->getStyle().getLight();
    if (light) {
        return Light::createJavaLightPeer(env, *map, *light);
    } else {
        return jni::Local<jni::Object<Light>>();
    }
}

jni::Local<jni::Array<jni::Object<Layer>>> NativeMapView::getLayers(JNIEnv& env) {

    // Get the core layers
    std::vector<style::Layer*> layers = map->getStyle().getLayers();

    // Convert
    auto jLayers = jni::Array<jni::Object<Layer>>::New(env, layers.size());
    int index = 0;
    for (auto layer : layers) {
        jLayers.Set(env, index, LayerManagerAndroid::get()->createJavaLayerPeer(env, *map, *layer));
        index++;
    }

    return jLayers;
}

jni::Local<jni::Object<Layer>> NativeMapView::getLayer(JNIEnv& env, const jni::String& layerId) {

    // Find the layer
    mbgl::style::Layer* coreLayer = map->getStyle().getLayer(jni::Make<std::string>(env, layerId));
    if (!coreLayer) {
       mbgl::Log::Debug(mbgl::Event::JNI, "No layer found");
       return jni::Local<jni::Object<Layer>>();
    }

    // Create and return the layer's native peer
    return LayerManagerAndroid::get()->createJavaLayerPeer(env, *map, *coreLayer);
}

void NativeMapView::addLayer(JNIEnv& env, jlong nativeLayerPtr, const jni::String& before) {
    assert(nativeLayerPtr != 0);

    Layer *layer = reinterpret_cast<Layer *>(nativeLayerPtr);
    try {
        layer->addToMap(*map, before ? mbgl::optional<std::string>(jni::Make<std::string>(env, before)) : mbgl::optional<std::string>());
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/CannotAddLayerException"), error.what());
    }
}

void NativeMapView::addLayerAbove(JNIEnv& env, jlong nativeLayerPtr, const jni::String& above) {
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
 * Remove layer at index.
 */
jni::jboolean NativeMapView::removeLayerAt(JNIEnv& env, jni::jint index) {
    auto layers = map->getStyle().getLayers();

    // Check index
    int numLayers = layers.size() - 1;
    if (index > numLayers || index < 0) {
        Log::Warning(Event::JNI, "Index out of range: %i", index);
        return jni::jni_false;
    }

    std::unique_ptr<mbgl::style::Layer> coreLayer = map->getStyle().removeLayer(layers.at(index)->getID());
    if (coreLayer) {
        jni::Local<jni::Object<Layer>> layerObj =
                LayerManagerAndroid::get()->createJavaLayerPeer(env, *map, std::move(coreLayer));
        return jni::jni_true;
    }
    return jni::jni_false;
}

/**
 * Remove with wrapper object id. Ownership is transferred back to the wrapper
 */
jni::jboolean NativeMapView::removeLayer(JNIEnv&, jlong layerPtr) {
    assert(layerPtr != 0);

    mbgl::android::Layer *layer = reinterpret_cast<mbgl::android::Layer *>(layerPtr);
    std::unique_ptr<mbgl::style::Layer> coreLayer = map->getStyle().removeLayer(layer->get().getID());
    if (coreLayer) {
        layer->setLayer(std::move(coreLayer));
        return jni::jni_true;
    }
    return jni::jni_false;
}

jni::Local<jni::Array<jni::Object<Source>>> NativeMapView::getSources(JNIEnv& env) {
    // Get the core sources
    std::vector<style::Source*> sources = map->getStyle().getSources();

    // Convert
    auto jSources = jni::Array<jni::Object<Source>>::New(env, sources.size());
    int index = 0;
    for (auto source : sources) {
        jSources.Set(env, index, Source::peerForCoreSource(env, *source, *rendererFrontend));
        index++;
    }

    return jSources;
}

jni::Local<jni::Object<Source>> NativeMapView::getSource(JNIEnv& env, const jni::String& sourceId) {
    // Find the source
    mbgl::style::Source* coreSource = map->getStyle().getSource(jni::Make<std::string>(env, sourceId));
    if (!coreSource) {
       mbgl::Log::Debug(mbgl::Event::JNI, "No source found");
       return jni::Local<jni::Object<Source>>();
    }

    // Create and return the source's native peer
    return jni::NewLocal(env, Source::peerForCoreSource(env, *coreSource, *rendererFrontend));
}

void NativeMapView::addSource(JNIEnv& env, const jni::Object<Source>& obj, jlong sourcePtr) {
    assert(sourcePtr != 0);

    Source *source = reinterpret_cast<Source *>(sourcePtr);
    try {
        source->addToMap(env, obj, *map, *rendererFrontend);
    } catch (const std::runtime_error& error) {
        jni::ThrowNew(env, jni::FindClass(env, "com/mapbox/mapboxsdk/style/sources/CannotAddSourceException"), error.what());
    }
}

jni::jboolean NativeMapView::removeSource(JNIEnv& env, const jni::Object<Source>& obj, jlong sourcePtr) {
    assert(sourcePtr != 0);

    mbgl::android::Source *source = reinterpret_cast<mbgl::android::Source *>(sourcePtr);
    if (source->removeFromMap(env, obj, *map)) {
        source->releaseJavaPeer();
        return jni::jni_true;
    }

    return jni::jni_false;
}

void NativeMapView::addImage(JNIEnv& env, const jni::String& name, const jni::Object<Bitmap>& bitmap, jni::jfloat scale, jni::jboolean sdf) {
    jni::NullCheck(env, &bitmap);
    mbgl::PremultipliedImage premultipliedImage = Bitmap::GetImage(env, bitmap);

    map->getStyle().addImage(std::make_unique<mbgl::style::Image>(
        jni::Make<std::string>(env, name),
        std::move(premultipliedImage),
        float(scale),
        sdf)
    );
}

void NativeMapView::addImages(JNIEnv& env, const jni::Array<jni::Object<mbgl::android::Image>>& jimages) {
    jni::NullCheck(env, &jimages);
    std::size_t len = jimages.Length(env);

    for (std::size_t i = 0; i < len; i++) {
        auto image = mbgl::android::Image::getImage(env, jimages.Get(env, i));
        map->getStyle().addImage(std::make_unique<mbgl::style::Image>(image));
    }
}

void NativeMapView::removeImage(JNIEnv& env, const jni::String& name) {
    map->getStyle().removeImage(jni::Make<std::string>(env, name));
}

jni::Local<jni::Object<Bitmap>> NativeMapView::getImage(JNIEnv& env, const jni::String& name) {
    const mbgl::style::Image *image = map->getStyle().getImage(jni::Make<std::string>(env, name));
    if (image) {
        return Bitmap::CreateBitmap(env, image->getImage());
    } else {
        return jni::Local<jni::Object<Bitmap>>();
    }
}

void NativeMapView::setPrefetchTiles(JNIEnv&, jni::jboolean enable) {
    map->setPrefetchZoomDelta(enable ? util::DEFAULT_PREFETCH_ZOOM_DELTA : uint8_t(0));
}

jni::jboolean NativeMapView::getPrefetchTiles(JNIEnv&) {
    return jni::jboolean(map->getPrefetchZoomDelta() > 0);
}

void NativeMapView::setPrefetchZoomDelta(JNIEnv&, jni::jint delta) {
    map->setPrefetchZoomDelta(uint8_t(delta));
}

jni::jint NativeMapView::getPrefetchZoomDelta(JNIEnv&) {
    return jni::jint(map->getPrefetchZoomDelta());
}

mbgl::Map& NativeMapView::getMap() {
    return *map;
}

// Static methods //

void NativeMapView::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    static auto& javaClass = jni::Class<NativeMapView>::Singleton(env);

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<NativeMapView>(
        env,
        javaClass,
        "nativePtr",
        jni::MakePeer<NativeMapView,
                      const jni::Object<NativeMapView>&,
                      const jni::Object<FileSource>&,
                      const jni::Object<MapRenderer>&,
                      jni::jfloat,
                      jni::jboolean>,
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
        METHOD(&NativeMapView::pixelsForLatLngs, "nativePixelsForLatLngs"),
        METHOD(&NativeMapView::latLngForProjectedMeters, "nativeLatLngForProjectedMeters"),
        METHOD(&NativeMapView::latLngForPixel, "nativeLatLngForPixel"),
        METHOD(&NativeMapView::latLngsForPixels, "nativeLatLngsForPixels"),
        METHOD(&NativeMapView::addPolylines, "nativeAddPolylines"),
        METHOD(&NativeMapView::addPolygons, "nativeAddPolygons"),
        METHOD(&NativeMapView::updatePolyline, "nativeUpdatePolyline"),
        METHOD(&NativeMapView::updatePolygon, "nativeUpdatePolygon"),
        METHOD(&NativeMapView::removeAnnotations, "nativeRemoveAnnotations"),
        METHOD(&NativeMapView::addAnnotationIcon, "nativeAddAnnotationIcon"),
        METHOD(&NativeMapView::removeAnnotationIcon, "nativeRemoveAnnotationIcon"),
        METHOD(&NativeMapView::getTopOffsetPixelsForAnnotationSymbol, "nativeGetTopOffsetPixelsForAnnotationSymbol"),
        METHOD(&NativeMapView::getTransitionOptions, "nativeGetTransitionOptions"),
        METHOD(&NativeMapView::setTransitionOptions, "nativeSetTransitionOptions"),
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
        METHOD(&NativeMapView::setPrefetchTiles, "nativeSetPrefetchTiles"),
        METHOD(&NativeMapView::getPrefetchTiles, "nativeGetPrefetchTiles"),
        METHOD(&NativeMapView::setPrefetchZoomDelta, "nativeSetPrefetchZoomDelta"),
        METHOD(&NativeMapView::getPrefetchZoomDelta, "nativeGetPrefetchZoomDelta"));
}

} // namespace android
} // namespace mbgl
