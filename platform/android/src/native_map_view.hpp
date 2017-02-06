#pragma once

#include <mbgl/map/backend.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>

#include "annotation/marker.hpp"
#include "annotation/polygon.hpp"
#include "annotation/polyline.hpp"
#include "graphics/pointF.hpp"
#include "graphics/rectF.hpp"
#include "geometry/feature.hpp"
#include "geometry/lat_lng.hpp"
#include "geometry/projected_meters.hpp"
#include "style/layers/layers.hpp"
#include "style/sources/sources.hpp"

#include <string>
#include <jni.h>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class NativeMapView : public View, public Backend {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/maps/NativeMapView"; };

    static jni::Class<NativeMapView> javaClass;

    static void registerNative(jni::JNIEnv&);

    NativeMapView(jni::JNIEnv&, jni::Object<NativeMapView>, jni::String, jni::String, jni::jfloat, jni::jint, jni::jlong);

    virtual ~NativeMapView();

    // mbgl::View //

    void bind() override;

    // mbgl::Backend //

    void invalidate() override;
    void notifyMapChange(mbgl::MapChange) override;

    // JNI //

    void destroy(jni::JNIEnv&);

    void render(jni::JNIEnv&);

    void onViewportChanged(jni::JNIEnv&, jni::jint width, jni::jint height);

    void setAPIBaseUrl(jni::JNIEnv&, jni::String);

    jni::String getStyleUrl(jni::JNIEnv&);

    void setStyleUrl(jni::JNIEnv&, jni::String);

    jni::String getStyleJson(jni::JNIEnv&);

    void setStyleJson(jni::JNIEnv&, jni::String);

    jni::String getAccessToken(jni::JNIEnv&);

    void setAccessToken(jni::JNIEnv&, jni::String);

    void cancelTransitions(jni::JNIEnv&);

    void setGestureInProgress(jni::JNIEnv&, jni::jboolean);

    void moveBy(jni::JNIEnv&, jni::jdouble, jni::jdouble);

    jni::Object<LatLng> getLatLng(JNIEnv&);

    void setLatLng(jni::JNIEnv&, jni::jdouble, jni::jdouble);

    void setReachability(jni::JNIEnv&, jni::jboolean);

    void resetPosition(jni::JNIEnv&);

    jni::jdouble getPitch(jni::JNIEnv&);

    void setPitch(jni::JNIEnv&, jni::jdouble);

    void scaleBy(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble);

    void setScale(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble);

    jni::jdouble getScale(jni::JNIEnv&);

    void setZoom(jni::JNIEnv&, jni::jdouble);

    jni::jdouble getZoom(jni::JNIEnv&);

    void resetZoom(jni::JNIEnv&);

    void setMinZoom(jni::JNIEnv&, jni::jdouble);

    jni::jdouble getMinZoom(jni::JNIEnv&);

    void setMaxZoom(jni::JNIEnv&, jni::jdouble);

    jni::jdouble getMaxZoom(jni::JNIEnv&);

    void rotateBy(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble, jni::jdouble);

    void setBearing(jni::JNIEnv&, jni::jdouble);

    void setBearingXY(jni::JNIEnv&, jni::jdouble, jni::jdouble, jni::jdouble);

    jni::jdouble getBearing(jni::JNIEnv&);

    void resetNorth(jni::JNIEnv&);

    void setVisibleCoordinateBounds(JNIEnv&, jni::Array<jni::Object<LatLng>>, jni::Object<RectF>, jdouble);

    void setContentPadding(JNIEnv&, double, double, double, double);

    void scheduleSnapshot(jni::JNIEnv&);

    void enableFps(jni::JNIEnv&, jni::jboolean enable);

    jni::Array<jni::jdouble> getCameraValues(jni::JNIEnv&);

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

    jdouble getTopOffsetPixelsForAnnotationSymbol(JNIEnv&, jni::String);

    jlong getTransitionDuration(JNIEnv&);

    void setTransitionDuration(JNIEnv&, jlong);

    jlong getTransitionDelay(JNIEnv&);

    void setTransitionDelay(JNIEnv&, jlong);

    jni::Array<jlong> queryPointAnnotations(JNIEnv&, jni::Object<RectF>);

    jni::Array<jni::Object<Feature>> queryRenderedFeaturesForPoint(JNIEnv&, jni::jfloat, jni::jfloat, jni::Array<jni::String>);

    jni::Array<jni::Object<Feature>> queryRenderedFeaturesForBox(JNIEnv&, jni::jfloat, jni::jfloat, jni::jfloat, jni::jfloat, jni::Array<jni::String>);

    jni::Object<Layer> getLayer(JNIEnv&, jni::String);

    void addLayer(JNIEnv&, jlong, jni::String);

    void removeLayerById(JNIEnv&, jni::String);

    void removeLayer(JNIEnv&, jlong);

    jni::Object<Source> getSource(JNIEnv&, jni::String);

    void addSource(JNIEnv&, jni::jlong);

    void removeSourceById(JNIEnv&, jni::String);

    void removeSource(JNIEnv&, jlong);

    void addImage(JNIEnv&, jni::String, jni::jint, jni::jint, jni::jfloat, jni::Array<jbyte>);

    void removeImage(JNIEnv&, jni::String);

protected:
    // Unused methods from mbgl::Backend //

    void activate() override {};
    void deactivate() override {};

private:
    void wake();
    void updateViewBinding();
    mbgl::Size getFramebufferSize() const;

    void resizeView(int width, int height);
    void resizeFramebuffer(int width, int height);

    void updateFps();

private:

    JavaVM *vm = nullptr;
    jni::UniqueWeakObject<NativeMapView> javaPeer;

    std::string styleUrl;
    std::string apiKey;

    float pixelRatio;
    bool fpsEnabled = false;
    bool snapshot = false;
    bool firstRender = true;
    double fps = 0.0;

    int width = 0;
    int height = 0;
    int fbWidth = 0;
    int fbHeight = 0;
    bool framebufferSizeChanged = true;

    int availableProcessors = 0;
    size_t totalMemory = 0;

    // Ensure these are initialised last
    std::unique_ptr<mbgl::util::RunLoop> runLoop;
    std::unique_ptr<mbgl::DefaultFileSource> fileSource;
    mbgl::ThreadPool threadPool;
    std::unique_ptr<mbgl::Map> map;
    mbgl::EdgeInsets insets;

};
}
}
