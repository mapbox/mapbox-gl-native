#pragma once

#include <mbgl/map/backend.hpp>
#include <mbgl/map/change.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>

#include "file_source.hpp"
#include "annotation/marker.hpp"
#include "annotation/polygon.hpp"
#include "annotation/polyline.hpp"
#include "graphics/pointf.hpp"
#include "graphics/rectf.hpp"
#include "geojson/feature.hpp"
#include "geometry/lat_lng.hpp"
#include "geometry/projected_meters.hpp"
#include "style/layers/layers.hpp"
#include "style/sources/sources.hpp"
#include "geometry/lat_lng_bounds.hpp"
#include "map/camera_position.hpp"
#include "style/light.hpp"

#include <exception>
#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <EGL/egl.h>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class NativeMapView : public View, public Backend, public MapObserver {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/maps/NativeMapView"; };

    static jni::Class<NativeMapView> javaClass;

    static void registerNative(jni::JNIEnv&);

    NativeMapView(jni::JNIEnv&,
                  jni::Object<NativeMapView>,
                  jni::Object<FileSource>,
                  jni::jfloat pixelRatio,
                  jni::String programCacheDir);

    virtual ~NativeMapView();

    // mbgl::View //

    void bind() override;

    // mbgl::Backend //

    void updateAssumedState() override;
    void invalidate() override;

    // Deprecated //
    void notifyMapChange(mbgl::MapChange);

    // mbgl::Backend (mbgl::MapObserver) //
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

    void destroy(jni::JNIEnv&);

    void render(jni::JNIEnv&);

    void update(jni::JNIEnv&);

    void resizeView(jni::JNIEnv&, int, int);

    void resizeFramebuffer(jni::JNIEnv&, int, int);

    void initializeDisplay(jni::JNIEnv&);

    void terminateDisplay(jni::JNIEnv&);

    void initializeContext(jni::JNIEnv&);

    void terminateContext(jni::JNIEnv&);

    void createSurface(jni::JNIEnv&, jni::Object<>);

    void destroySurface(jni::JNIEnv&);

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

    void enableFps(jni::JNIEnv&, jni::jboolean enable);

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

    jni::jdouble getTopOffsetPixelsForAnnotationSymbol(JNIEnv&, jni::String);

    jni::jlong getTransitionDuration(JNIEnv&);

    void setTransitionDuration(JNIEnv&, jni::jlong);

    jni::jlong getTransitionDelay(JNIEnv&);

    void setTransitionDelay(JNIEnv&, jni::jlong);

    jni::Array<jlong> queryPointAnnotations(JNIEnv&, jni::Object<RectF>);

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

    void addSource(JNIEnv&, jni::jlong);

    jni::Object<Source> removeSourceById(JNIEnv&, jni::String);

    void removeSource(JNIEnv&, jlong);

    void addImage(JNIEnv&, jni::String, jni::jint, jni::jint, jni::jfloat, jni::Array<jbyte>);

    void removeImage(JNIEnv&, jni::String);

protected:
    // mbgl::Backend //

    gl::ProcAddress initializeExtension(const char*) override;
    void activate() override;
    void deactivate() override;

private:
    void _initializeDisplay();

    void _terminateDisplay();

    void _initializeContext();

    void _terminateContext();

    void _createSurface(ANativeWindow*);

    void _destroySurface();

    EGLConfig chooseConfig(const EGLConfig configs[], EGLint numConfigs);

    mbgl::Size getFramebufferSize() const;

    void updateFps();

private:
    JavaVM *vm = nullptr;
    jni::UniqueWeakObject<NativeMapView> javaPeer;

    std::string styleUrl;
    std::string apiKey;

    ANativeWindow *window = nullptr;

    EGLConfig config = nullptr;
    EGLint format = -1;

    EGLDisplay oldDisplay = EGL_NO_DISPLAY;
    EGLSurface oldReadSurface = EGL_NO_SURFACE;
    EGLSurface oldDrawSurface = EGL_NO_SURFACE;
    EGLContext oldContext = EGL_NO_CONTEXT;

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;


    float pixelRatio;
    bool fpsEnabled = false;
    bool snapshot = false;
    bool firstRender = true;
    double fps = 0.0;

    // Minimum texture size according to OpenGL ES 2.0 specification.
    int width = 64;
    int height = 64;
    int fbWidth = 64;
    int fbHeight = 64;

    bool framebufferSizeChanged = true;

    // Ensure these are initialised last
    std::shared_ptr<mbgl::ThreadPool> threadPool;
    std::unique_ptr<mbgl::Map> map;
    mbgl::EdgeInsets insets;

    unsigned active = 0;
};

} // namespace android
} // namespace mbgl
