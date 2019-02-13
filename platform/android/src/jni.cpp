#include "jni.hpp"

#include <mbgl/util/logging.hpp>

#ifndef MBGL_MODULE_ANNOTATION_DISABLE
#include "annotation/marker.hpp"
#include "annotation/polygon.hpp"
#include "annotation/polyline.hpp"
#endif
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
#ifndef MBGL_GEOJSON_LINESTRING_DISABLE
#include "geojson/line_string.hpp"
#endif
#ifndef MBGL_GEOJSON_MULTILINESTRING_DISABLE
#include "geojson/multi_line_string.hpp"
#endif
#ifndef MBGL_GEOJSON_MULTIPOINT_DISABLE
#include "geojson/multi_point.hpp"
#endif
#ifndef MBGL_GEOJSON_MULTIPOLYGON_DISABLE
#include "geojson/multi_polygon.hpp"
#endif
#include "geojson/point.hpp"
#ifndef MBGL_GEOJSON_POLYGON_DISABLE
#include "geojson/polygon.hpp"
#endif
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
#ifndef MBGL_MODULE_OFFLINE_DISABLE
#include "offline/offline_manager.hpp"
#include "offline/offline_region.hpp"
#include "offline/offline_region_definition.hpp"
#include "offline/offline_region_error.hpp"
#include "offline/offline_region_status.hpp"
#endif
#include "style/transition_options.hpp"
#include "style/layers/layer_manager.hpp"
#include "style/sources/source.hpp"
#ifndef MBGL_STYLE_LIGHT_DISABLE
#include "style/light.hpp"
#endif
#include "style/formatted.hpp"
#include "style/formatted_section.hpp"
#ifndef MBGL_MODULE_SNAPSHOT_DISABLE
#include "snapshotter/map_snapshotter.hpp"
#include "snapshotter/map_snapshot.hpp"
#endif
#include "text/collator_jni.hpp"
#include "text/local_glyph_rasterizer_jni.hpp"
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

    // GeoJSON
    geojson::Feature::registerNative(env);
    geojson::FeatureCollection::registerNative(env);
    geojson::Geometry::registerNative(env);
    geojson::GeometryCollection::registerNative(env);
#ifndef MBGL_GEOJSON_LINESTRING_DISABLE
    geojson::LineString::registerNative(env);
#endif
#ifndef MBGL_GEOJSON_MULTILINESTRING_DISABLE
    geojson::MultiLineString::registerNative(env);
#endif
#ifndef MBGL_GEOJSON_MULTIPOINT_DISABLE
    geojson::MultiPoint::registerNative(env);
#endif
#ifndef MBGL_GEOJSON_MULTIPOLYGON_DISABLE
    geojson::MultiPolygon::registerNative(env);
#endif
    geojson::Point::registerNative(env);
#ifndef MBGL_GEOJSON_POLYGON_DISABLE
    geojson::Polygon::registerNative(env);
#endif

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
#ifndef MBGL_MODULE_ANNOTATION_DISABLE
    Marker::registerNative(env);
    Polygon::registerNative(env);
    Polyline::registerNative(env);
#endif

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
    LayerManagerAndroid::get()->registerNative(env);
    Source::registerNative(env);
#ifndef MBGL_STYLE_LIGHT_DISABLE
    Light::registerNative(env);
#endif
#ifndef MBGL_STYLE_POSITION_DISABLE
    Position::registerNative(env);
#endif
    Formatted::registerNative(env);
    FormattedSection::registerNative(env);

    // Map
    CameraPosition::registerNative(env);
    Image::registerNative(env);

    // Connectivity
    ConnectivityListener::registerNative(env);

    // Offline
#ifndef MBGL_MODULE_OFFLINE_DISABLE
    OfflineManager::registerNative(env);
    OfflineRegion::registerNative(env);
    OfflineRegionDefinition::registerNative(env);
    OfflineTilePyramidRegionDefinition::registerNative(env);
    OfflineGeometryRegionDefinition::registerNative(env);
    OfflineRegionError::registerNative(env);
    OfflineRegionStatus::registerNative(env);
#endif

    // Snapshotter
#ifndef MBGL_MODULE_SNAPSHOT_DISABLE
    MapSnapshotter::registerNative(env);
    MapSnapshot::registerNative(env);
#endif

    // text
    LocalGlyphRasterizer::registerNative(env);
    Locale::registerNative(env);
    Collator::registerNative(env);
    StringUtils::registerNative(env);

    // Logger
    Logger::registerNative(env);
}

} // namespace android
} // namespace mbgl
