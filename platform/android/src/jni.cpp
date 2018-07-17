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
#include "geojson/feature.hpp"
#include "geojson/feature_collection.hpp"
#include "geojson/geometry.hpp"
#include "geojson/geometry_collection.hpp"
#include "geojson/line_string.hpp"
#include "geojson/multi_line_string.hpp"
#include "geojson/multi_point.hpp"
#include "geojson/multi_polygon.hpp"
#include "geojson/point.hpp"
#include "geojson/polygon.hpp"
#include "geometry/lat_lng.hpp"
#include "geometry/lat_lng_bounds.hpp"
#include "geometry/lat_lng_quad.hpp"
#include "geometry/projected_meters.hpp"
#include "graphics/pointf.hpp"
#include "graphics/rectf.hpp"
#include "gson/json_array.hpp"
#include "gson/json_element.hpp"
#include "gson/json_object.hpp"
#include "gson/json_primitive.hpp"
#include "java_types.hpp"
#include "map_renderer.hpp"
#include "map_renderer_runnable.hpp"
#include "native_map_view.hpp"
#include "offline/offline_manager.hpp"
#include "offline/offline_region.hpp"
#include "offline/offline_region_definition.hpp"
#include "offline/offline_region_error.hpp"
#include "offline/offline_region_status.hpp"
#include "style/transition_options.hpp"
#include "style/layers/layers.hpp"
#include "style/sources/source.hpp"
#include "style/light.hpp"
#include "snapshotter/map_snapshotter.hpp"
#include "snapshotter/map_snapshot.hpp"
#include "text/collator_jni.hpp"
#include "text/local_glyph_rasterizer_jni.hpp"
#include "java/lang.hpp"
#include "logger.hpp"

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
    java::lang::Number::registerNative(env);
    java::lang::Float::registerNative(env);
    java::lang::Boolean::registerNative(env);
    java::lang::Double::registerNative(env);
    java::lang::Long::registerNative(env);

    // GeoJSON
    geojson::Feature::registerNative(env);
    geojson::FeatureCollection::registerNative(env);
    geojson::Geometry::registerNative(env);
    geojson::GeometryCollection::registerNative(env);
    geojson::LineString::registerNative(env);
    geojson::MultiLineString::registerNative(env);
    geojson::MultiPoint::registerNative(env);
    geojson::MultiPolygon::registerNative(env);
    geojson::Point::registerNative(env);
    geojson::Polygon::registerNative(env);

    // Geometry
    LatLng::registerNative(env);
    LatLngBounds::registerNative(env);
    LatLngQuad::registerNative(env);
    ProjectedMeters::registerNative(env);

    // GSon
    gson::JsonArray::registerNative(env);
    gson::JsonElement::registerNative(env);
    gson::JsonObject::registerNative(env);
    gson::JsonPrimitive::registerNative(env);

    //Annotation
    Marker::registerNative(env);
    Polygon::registerNative(env);
    Polyline::registerNative(env);

    // Map
    MapRenderer::registerNative(env);
    MapRendererRunnable::registerNative(env);
    NativeMapView::registerNative(env);

    // Http
    RegisterNativeHTTPRequest(env);

    // Bitmap
    Bitmap::registerNative(env);
    BitmapFactory::registerNative(env);

    // Style
    TransitionOptions::registerNative(env);
    registerNativeLayers(env);
    Source::registerNative(env);
    Light::registerNative(env);
    Position::registerNative(env);

    // Map
    CameraPosition::registerNative(env);
    Image::registerNative(env);

    // Connectivity
    ConnectivityListener::registerNative(env);

    // Offline
    OfflineManager::registerNative(env);
    OfflineRegion::registerNative(env);
    OfflineRegionDefinition::registerNative(env);
    OfflineTilePyramidRegionDefinition::registerNative(env);
    OfflineGeometryRegionDefinition::registerNative(env);
    OfflineRegionError::registerNative(env);
    OfflineRegionStatus::registerNative(env);

    // Snapshotter
    MapSnapshotter::registerNative(env);
    MapSnapshot::registerNative(env);

    // text
    LocalGlyphRasterizer::registerNative(env);
    Locale::registerNative(env);
    Collator::registerNative(env);

    // Logger
    Logger::registerNative(env);
}

} // namespace android
} // namespace mbgl
