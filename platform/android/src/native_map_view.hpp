#pragma once

#include <mbgl/map/change.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/storage/network_status.hpp>

#include "annotation/marker.hpp"
#include "annotation/polygon.hpp"
#include "annotation/polyline.hpp"
#include "graphics/pointf.hpp"
#include "graphics/rectf.hpp"
#include "geojson/feature.hpp"
#include "geojson/geometry.hpp"
#include "geometry/lat_lng.hpp"
#include "geometry/projected_meters.hpp"
#include "style/layers/layer_manager.hpp"
#include "style/sources/source.hpp"
#include "geometry/lat_lng_bounds.hpp"
#include "map/camera_position.hpp"
#include "map/image.hpp"
#include "style/light.hpp"
#include "bitmap.hpp"

#include <exception>
#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <EGL/egl.h>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class AndroidRendererFrontend;
class FileSource;
class MapRenderer;

class NativeMapView : public MapObserver {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/maps/NativeMapView"; };

    static void registerNative(jni::JNIEnv&);

    NativeMapView(jni::JNIEnv&,
                  const jni::Object<NativeMapView>&,
                  const jni::Object<FileSource>&,
                  const jni::Object<MapRenderer>&,
                  jni::jfloat,
                  jni::jboolean);

    virtual ~NativeMapView();

    // mbgl::RendererBackend (mbgl::MapObserver) //
    void onCameraWillChange(MapObserver::CameraChangeMode) override;
    void onCameraIsChanging() override;
    void onCameraDidChange(MapObserver::CameraChangeMode) override;
    void onWillStartLoadingMap() override;
    void onDidFinishLoadingMap() override;
    void onDidFailLoadingMap(MapLoadError, const std::string&) override;
    void onWillStartRenderingFrame() override;
    void onDidFinishRenderingFrame(MapObserver::RenderFrameStatus) override;
    void onWillStartRenderingMap() override;
    void onDidFinishRenderingMap(MapObserver::RenderMode) override;
    void onDidBecomeIdle() override;
    void onDidFinishLoadingStyle() override;
    void onSourceChanged(mbgl::style::Source&) override;
    void onStyleImageMissing(const std::string&) override;
    bool onCanRemoveUnusedStyleImage(const std::string&) override;

    // JNI //

    void resizeView(jni::JNIEnv&, int, int);

    jni::Local<jni::String> getStyleUrl(jni::JNIEnv&);

    void setStyleUrl(jni::JNIEnv&, const jni::String&);

    jni::Local<jni::String> getStyleJson(jni::JNIEnv&);

    void setStyleJson(jni::JNIEnv&, const jni::String&);

    void setLatLngBounds(jni::JNIEnv&, const jni::Object<mbgl::android::LatLngBounds>&);

    void cancelTransitions(jni::JNIEnv&);

    void setGestureInProgress(jni::JNIEnv&, jni::jboolean);

    void moveBy(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jlong);

    void jumpTo(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble, jni::jdouble, jni::jdouble, const jni::Array<jni::jdouble>&);

    void easeTo(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble, jni::jlong, jni::jdouble, jni::jdouble, const jni::Array<jni::jdouble>&, jni::jboolean);

    void flyTo(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble, jni::jlong, jni::jdouble, jni::jdouble, const jni::Array<jni::jdouble>&);

    jni::Local<jni::Object<LatLng>> getLatLng(JNIEnv&);

    void setLatLng(jni::JNIEnv&, jni::jdouble, jni::jdouble, const jni::Array<jni::jdouble>&, jni::jlong);

    jni::Local<jni::Object<CameraPosition>> getCameraForLatLngBounds(jni::JNIEnv&, const jni::Object<mbgl::android::LatLngBounds>&, double top, double left, double bottom, double right, double bearing, double tilt);

    jni::Local<jni::Object<CameraPosition>> getCameraForGeometry(jni::JNIEnv&, const jni::Object<geojson::Geometry>&, double top, double left, double bottom, double right, double bearing, double tilt);

    void setReachability(jni::JNIEnv&, jni::jboolean);

    void resetPosition(jni::JNIEnv&);

    jni::jdouble getPitch(jni::JNIEnv&);

    void setPitch(jni::JNIEnv&, jni::jdouble, jni::jlong);

    void setZoom(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble, jni::jlong);

    jni::jdouble getZoom(jni::JNIEnv&);

    void resetZoom(jni::JNIEnv&);

    void setMinZoom(jni::JNIEnv&, jni::jdouble);

    jni::jdouble getMinZoom(jni::JNIEnv&);

    void setMaxZoom(jni::JNIEnv&, jni::jdouble);

    jni::jdouble getMaxZoom(jni::JNIEnv&);

    void rotateBy(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble, jni::jdouble, jni::jlong);

    void setBearing(jni::JNIEnv&, jni::jdouble, jni::jlong);

    void setBearingXY(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble, jni::jlong);

    jni::jdouble getBearing(jni::JNIEnv&);

    void resetNorth(jni::JNIEnv&);

    void setVisibleCoordinateBounds(JNIEnv&, const jni::Array<jni::Object<LatLng>>&, const jni::Object<RectF>&, jni::jdouble, jni::jlong);

    void scheduleSnapshot(jni::JNIEnv&);

    jni::Local<jni::Object<CameraPosition>> getCameraPosition(jni::JNIEnv&);

    void updateMarker(jni::JNIEnv&, jni::jlong, jni::jdouble, jni::jdouble, const jni::String&);

    jni::Local<jni::Array<jni::jlong>> addMarkers(jni::JNIEnv&, const jni::Array<jni::Object<Marker>>&);

    void onLowMemory(JNIEnv& env);

    void setDebug(JNIEnv&, jni::jboolean);

    void cycleDebugOptions(JNIEnv&);

    jni::jboolean getDebug(JNIEnv&);

    jni::jboolean isFullyLoaded(JNIEnv&);

    jni::jdouble getMetersPerPixelAtLatitude(JNIEnv&, jni::jdouble, jni::jdouble);

    jni::Local<jni::Object<ProjectedMeters>> projectedMetersForLatLng(JNIEnv&, jni::jdouble, jni::jdouble);

    jni::Local<jni::Object<PointF>> pixelForLatLng(JNIEnv&, jdouble, jdouble);

    void pixelsForLatLngs(JNIEnv&, const jni::Array<jdouble>&, jni::Array<jdouble>&, jfloat);

    jni::Local<jni::Object<LatLng>> latLngForProjectedMeters(JNIEnv&, jdouble, jdouble);

    jni::Local<jni::Object<LatLng>> latLngForPixel(JNIEnv&, jfloat, jfloat);

    void latLngsForPixels(JNIEnv&, const jni::Array<jdouble>&, jni::Array<jdouble>&, jfloat);

    jni::Local<jni::Array<jlong>> addPolylines(JNIEnv&, const jni::Array<jni::Object<Polyline>>&);

    jni::Local<jni::Array<jlong>> addPolygons(JNIEnv&, const jni::Array<jni::Object<Polygon>>&);

    void updatePolyline(JNIEnv&, jlong, const jni::Object<Polyline>&);

    void updatePolygon(JNIEnv&, jlong, const jni::Object<Polygon>&);

    void removeAnnotations(JNIEnv&, const jni::Array<jlong>&);

    void addAnnotationIcon(JNIEnv&, const jni::String&, jint, jint, jfloat, const jni::Array<jbyte>&);

    void removeAnnotationIcon(JNIEnv&, const jni::String&);

    jni::jdouble getTopOffsetPixelsForAnnotationSymbol(JNIEnv&, const jni::String&);

    jni::Local<jni::Object<TransitionOptions>> getTransitionOptions(JNIEnv&);

    void setTransitionOptions(JNIEnv&, const jni::Object<TransitionOptions>&);

    jni::Local<jni::Array<jlong>> queryPointAnnotations(JNIEnv&, const jni::Object<RectF>&);

    jni::Local<jni::Array<jlong>> queryShapeAnnotations(JNIEnv&, const jni::Object<RectF>&);

    jni::Local<jni::Array<jni::Object<geojson::Feature>>> queryRenderedFeaturesForPoint(JNIEnv&, jni::jfloat, jni::jfloat,
                                                                   const jni::Array<jni::String>&,
                                                                   const jni::Array<jni::Object<>>& jfilter);

    jni::Local<jni::Array<jni::Object<geojson::Feature>>> queryRenderedFeaturesForBox(JNIEnv&, jni::jfloat, jni::jfloat, jni::jfloat,
                                                                 jni::jfloat, const jni::Array<jni::String>&,
                                                                 const jni::Array<jni::Object<>>& jfilter);

    jni::Local<jni::Object<Light>> getLight(JNIEnv&);

    jni::Local<jni::Array<jni::Object<Layer>>> getLayers(JNIEnv&);

    jni::Local<jni::Object<Layer>> getLayer(JNIEnv&, const jni::String&);

    void addLayer(JNIEnv&, jlong, const jni::String&);

    void addLayerAbove(JNIEnv&, jlong, const jni::String&);

    void addLayerAt(JNIEnv&, jni::jlong, jni::jint);

    jni::jboolean removeLayerAt(JNIEnv&, jni::jint);

    jni::jboolean removeLayer(JNIEnv&, jlong);

    jni::Local<jni::Array<jni::Object<Source>>> getSources(JNIEnv&);

    jni::Local<jni::Object<Source>> getSource(JNIEnv&, const jni::String&);

    void addSource(JNIEnv&, const jni::Object<Source>&, jlong nativePtr);

    jni::jboolean removeSource(JNIEnv&, const jni::Object<Source>&, jlong nativePtr);

    void addImage(JNIEnv&, const jni::String&, const jni::Object<Bitmap>& bitmap, jni::jfloat, jni::jboolean);

    void addImages(JNIEnv&, const jni::Array<jni::Object<mbgl::android::Image>>&);

    void removeImage(JNIEnv&, const jni::String&);

    jni::Local<jni::Object<Bitmap>> getImage(JNIEnv&, const jni::String&);

    void setPrefetchTiles(JNIEnv&, jni::jboolean);

    jni::jboolean getPrefetchTiles(JNIEnv&);

    void setPrefetchZoomDelta(JNIEnv&, jni::jint);

    jni::jint getPrefetchZoomDelta(JNIEnv&);

    mbgl::Map& getMap();

private:
    std::unique_ptr<AndroidRendererFrontend> rendererFrontend;

    JavaVM *vm = nullptr;
    jni::WeakReference<jni::Object<NativeMapView>> javaPeer;

    MapRenderer& mapRenderer;

    std::string styleUrl;

    float pixelRatio;

    // Minimum texture size according to OpenGL ES 2.0 specification.
    int width = 64;
    int height = 64;

    // Ensure these are initialised last
    std::unique_ptr<mbgl::Map> map;
};

} // namespace android
} // namespace mbgl
