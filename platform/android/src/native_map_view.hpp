#pragma once

#include <mbgl/map/change.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/storage/default_file_source.hpp>
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
#include "style/layers/layers.hpp"
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

    static jni::Class<NativeMapView> javaClass;

    static void registerNative(jni::JNIEnv&);

    NativeMapView(jni::JNIEnv&,
                  jni::Object<NativeMapView>,
                  jni::Object<FileSource>,
                  jni::Object<MapRenderer>,
                  jni::jfloat pixelRatio);

    virtual ~NativeMapView();

    // Deprecated //
    void notifyMapChange(mbgl::MapChange);

    // mbgl::RendererBackend (mbgl::MapObserver) //
    void onCameraWillChange(MapObserver::CameraChangeMode) override;
    void onCameraIsChanging() override;
    void onCameraDidChange(MapObserver::CameraChangeMode) override;
    void onWillStartLoadingMap() override;
    void onDidFinishLoadingMap() override;
    void onDidFailLoadingMap(std::exception_ptr) override;
    void onWillStartRenderingFrame() override;
    void onDidFinishRenderingFrame(MapObserver::RenderMode) override;
    void onWillStartRenderingMap() override;
    void onDidFinishRenderingMap(MapObserver::RenderMode) override;
    void onDidFinishLoadingStyle() override;
    void onSourceChanged(mbgl::style::Source&) override;

    // JNI //

    void resizeView(jni::JNIEnv&, int, int);

    jni::String getStyleUrl(jni::JNIEnv&);

    void setStyleUrl(jni::JNIEnv&, jni::String);

    jni::String getStyleJson(jni::JNIEnv&);

    void setStyleJson(jni::JNIEnv&, jni::String);

    void setLatLngBounds(jni::JNIEnv&, jni::Object<mbgl::android::LatLngBounds>);

    void cancelTransitions(jni::JNIEnv&);

    void setGestureInProgress(jni::JNIEnv&, jni::jboolean);

    void moveBy(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jlong);

    void jumpTo(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble, jni::jdouble, jni::jdouble);

    void easeTo(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble, jni::jlong, jni::jdouble, jni::jdouble, jni::jboolean);

    void flyTo(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble, jni::jlong, jni::jdouble, jni::jdouble);

    jni::Object<LatLng> getLatLng(JNIEnv&);

    void setLatLng(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jlong);

    jni::Object<CameraPosition> getCameraForLatLngBounds(jni::JNIEnv&, jni::Object<mbgl::android::LatLngBounds>);

    jni::Object<CameraPosition> getCameraForGeometry(jni::JNIEnv&, jni::Object<geojson::Geometry>, double bearing);

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

    void setVisibleCoordinateBounds(JNIEnv&, jni::Array<jni::Object<LatLng>>, jni::Object<RectF>, jni::jdouble, jni::jlong);

    void setContentPadding(JNIEnv&, double, double, double, double);

    void scheduleSnapshot(jni::JNIEnv&);

    jni::Object<CameraPosition> getCameraPosition(jni::JNIEnv&);

    void updateMarker(jni::JNIEnv&, jni::jlong, jni::jdouble, jni::jdouble, jni::String);

    jni::Array<jni::jlong> addMarkers(jni::JNIEnv&, jni::Array<jni::Object<Marker>>);

    void onLowMemory(JNIEnv& env);

    void setDebug(JNIEnv&, jni::jboolean);

    void cycleDebugOptions(JNIEnv&);

    jni::jboolean getDebug(JNIEnv&);

    jni::jboolean isFullyLoaded(JNIEnv&);

    jni::jdouble getMetersPerPixelAtLatitude(JNIEnv&, jni::jdouble, jni::jdouble);

    jni::Object<ProjectedMeters> projectedMetersForLatLng(JNIEnv&, jni::jdouble, jni::jdouble);

    jni::Object<PointF> pixelForLatLng(JNIEnv&, jdouble, jdouble);

    jni::Object<LatLng> latLngForProjectedMeters(JNIEnv&, jdouble, jdouble);

    jni::Object<LatLng> latLngForPixel(JNIEnv&, jfloat, jfloat);

    jni::Array<jlong> addPolylines(JNIEnv&, jni::Array<jni::Object<Polyline>>);

    jni::Array<jlong> addPolygons(JNIEnv&, jni::Array<jni::Object<Polygon>>);

    void updatePolyline(JNIEnv&, jlong, jni::Object<Polyline>);

    void updatePolygon(JNIEnv&, jlong, jni::Object<Polygon>);

    void removeAnnotations(JNIEnv&, jni::Array<jlong>);

    void addAnnotationIcon(JNIEnv&, jni::String, jint, jint, jfloat, jni::Array<jbyte>);

    void removeAnnotationIcon(JNIEnv&, jni::String);

    jni::jdouble getTopOffsetPixelsForAnnotationSymbol(JNIEnv&, jni::String);

    jni::jlong getTransitionDuration(JNIEnv&);

    void setTransitionDuration(JNIEnv&, jni::jlong);

    jni::jlong getTransitionDelay(JNIEnv&);

    void setTransitionDelay(JNIEnv&, jni::jlong);

    jni::Array<jlong> queryPointAnnotations(JNIEnv&, jni::Object<RectF>);

    jni::Array<jlong> queryShapeAnnotations(JNIEnv&, jni::Object<RectF>);

    jni::Array<jni::Object<geojson::Feature>> queryRenderedFeaturesForPoint(JNIEnv&, jni::jfloat, jni::jfloat,
                                                                   jni::Array<jni::String>,
                                                                   jni::Array<jni::Object<>> jfilter);

    jni::Array<jni::Object<geojson::Feature>> queryRenderedFeaturesForBox(JNIEnv&, jni::jfloat, jni::jfloat, jni::jfloat,
                                                                 jni::jfloat, jni::Array<jni::String>,
                                                                 jni::Array<jni::Object<>> jfilter);

    jni::Object<Light> getLight(JNIEnv&);

    jni::Array<jni::Object<Layer>> getLayers(JNIEnv&);

    jni::Object<Layer> getLayer(JNIEnv&, jni::String);

    void addLayer(JNIEnv&, jlong, jni::String);

    void addLayerAbove(JNIEnv&, jlong, jni::String);

    void addLayerAt(JNIEnv&, jni::jlong, jni::jint);

    jni::Object<Layer> removeLayerById(JNIEnv&, jni::String);

    jni::Object<Layer> removeLayerAt(JNIEnv&, jni::jint);

    void removeLayer(JNIEnv&, jlong);

    jni::Array<jni::Object<Source>> getSources(JNIEnv&);

    jni::Object<Source> getSource(JNIEnv&, jni::String);

    void addSource(JNIEnv&, jni::Object<Source>, jlong nativePtr);

    jni::Object<Source> removeSourceById(JNIEnv&, jni::String);

    void removeSource(JNIEnv&, jni::Object<Source>, jlong nativePtr);

    void addImage(JNIEnv&, jni::String, jni::jint, jni::jint, jni::jfloat, jni::Array<jbyte>);

    void addImages(JNIEnv&, jni::Array<jni::Object<mbgl::android::Image>>);

    void removeImage(JNIEnv&, jni::String);

    jni::Object<Bitmap> getImage(JNIEnv&, jni::String);

    void setPrefetchesTiles(JNIEnv&, jni::jboolean);

    jni::jboolean getPrefetchesTiles(JNIEnv&);

private:
    std::unique_ptr<AndroidRendererFrontend> rendererFrontend;

    JavaVM *vm = nullptr;
    jni::UniqueWeakObject<NativeMapView> javaPeer;

    MapRenderer& mapRenderer;

    std::string styleUrl;
    std::string apiKey;

    float pixelRatio;

    // Minimum texture size according to OpenGL ES 2.0 specification.
    int width = 64;
    int height = 64;

    // Ensure these are initialised last
    std::shared_ptr<mbgl::ThreadPool> threadPool;
    std::unique_ptr<mbgl::Map> map;
    mbgl::EdgeInsets insets;
};

} // namespace android
} // namespace mbgl
