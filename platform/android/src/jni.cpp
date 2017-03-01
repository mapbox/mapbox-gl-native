#include "jni.hpp"

#include <mbgl/util/logging.hpp>

#include "annotation/marker.hpp"
#include "annotation/polygon.hpp"
#include "annotation/polyline.hpp"
#include "bitmap.hpp"
#include "bitmap_factory.hpp"
#include "connectivity_listener.hpp"
#include "conversion/conversion.hpp"
#include "conversion/collection.hpp"
#include "file_source.hpp"
#include "geometry/feature.hpp"
#include "geometry/lat_lng.hpp"
#include "geometry/lat_lng_bounds.hpp"
#include "geometry/projected_meters.hpp"
#include "graphics/pointf.hpp"
#include "graphics/rectf.hpp"
#include "java_types.hpp"
#include "native_map_view.hpp"
#include "offline/offline_manager.hpp"
#include "offline/offline_region.hpp"
#include "offline/offline_region_definition.hpp"
#include "offline/offline_region_error.hpp"
#include "offline/offline_region_status.hpp"
#include "style/functions/categorical_stops.hpp"
#include "style/functions/exponential_stops.hpp"
#include "style/functions/identity_stops.hpp"
#include "style/functions/interval_stops.hpp"
#include "style/functions/stop.hpp"
#include "style/layers/layers.hpp"
#include "style/sources/sources.hpp"

namespace mbgl {
namespace android {

void RegisterNativeHTTPRequest(JNIEnv&);

JavaVM* theJVM;

//TODO: remove
bool attach_jni_thread(JavaVM* vm, JNIEnv** env, std::string threadName) {
    assert(vm != nullptr);
    assert(env != nullptr);

    JavaVMAttachArgs args = {JNI_VERSION_1_2, threadName.c_str(), NULL};

    jint ret;
    *env = nullptr;
    bool detach = false;
    ret = vm->GetEnv(reinterpret_cast<void **>(env), JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        if (ret != JNI_EDETACHED) {
            mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
            throw std::runtime_error("GetEnv() failed");
        } else {
            ret = vm->AttachCurrentThread(env, &args);
            if (ret != JNI_OK) {
                mbgl::Log::Error(mbgl::Event::JNI, "AttachCurrentThread() failed with %i", ret);
                throw std::runtime_error("AttachCurrentThread() failed");
            }
            detach = true;
        }
    }

    return detach;
}

//TODO: remove
void detach_jni_thread(JavaVM* vm, JNIEnv** env, bool detach) {
    if (detach) {
        assert(vm != nullptr);
        assert(env != nullptr);

        jint ret;
        if ((ret = vm->DetachCurrentThread()) != JNI_OK) {
            mbgl::Log::Error(mbgl::Event::JNI, "DetachCurrentThread() failed with %i", ret);
            throw std::runtime_error("DetachCurrentThread() failed");
        }
    }
    *env = nullptr;
}

void registerNatives(JavaVM *vm) {
    theJVM = vm;

    jni::JNIEnv& env = jni::GetEnv(*vm, jni::jni_version_1_6);

    // For the DefaultFileSource
    static mbgl::util::RunLoop mainRunLoop;
    FileSource::registerNative(env);

    // Basic types
    java::registerNatives(env);
    java::util::registerNative(env);
    PointF::registerNative(env);
    RectF::registerNative(env);

    // Geometry
    Feature::registerNative(env);
    LatLng::registerNative(env);
    LatLngBounds::registerNative(env);
    ProjectedMeters::registerNative(env);

    //Annotation
    Marker::registerNative(env);
    Polygon::registerNative(env);
    Polyline::registerNative(env);

    // Map
    NativeMapView::registerNative(env);

    // Http
    RegisterNativeHTTPRequest(env);

    // Bitmap
    Bitmap::registerNative(env);
    BitmapFactory::registerNative(env);

    // Style
    registerNativeLayers(env);
    registerNativeSources(env);
    Stop::registerNative(env);
    CategoricalStops::registerNative(env);
    ExponentialStops::registerNative(env);
    IdentityStops::registerNative(env);
    IntervalStops::registerNative(env);

    // Connectivity
    ConnectivityListener::registerNative(env);

    // Offline
    OfflineManager::registerNative(env);
    OfflineRegion::registerNative(env);
    OfflineRegionDefinition::registerNative(env);
    OfflineTilePyramidRegionDefinition::registerNative(env);
    OfflineRegionError::registerNative(env);
    OfflineRegionStatus::registerNative(env);
}

} // namespace android
} // namespace mbgl
