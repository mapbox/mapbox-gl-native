#include "jni_native.hpp"

#include "annotation/marker.hpp"
#include "annotation/polygon.hpp"
#include "annotation/polyline.hpp"
#include "bitmap.hpp"
#include "bitmap_factory.hpp"
#include "connectivity_listener.hpp"
#include "conversion/collection.hpp"
#include "conversion/conversion.hpp"
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
#include "mapbox.hpp"
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
#include "style/light.hpp"
#include "style/formatted.hpp"
#include "style/formatted_section.hpp"
#ifndef MBGL_MODULE_SNAPSHOT_DISABLE
#include "snapshotter/map_snapshotter.hpp"
#include "snapshotter/map_snapshot.hpp"
#endif
#include "i18n/collator_jni.hpp"
#include "i18n/number_format_jni.hpp"
#include "logger.hpp"
#include "text/local_glyph_rasterizer_jni.hpp"

namespace mbgl {
namespace android {

void RegisterNativeHTTPRequest(JNIEnv&);

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
    LayerManagerAndroid::get()->registerNative(env);
    Source::registerNative(env);
    Light::registerNative(env);
    Position::registerNative(env);
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
    NumberFormat::registerNative(env);

    // Logger
    Logger::registerNative(env);

    // AssetManager
    Mapbox::registerNative(env);
}

} // namespace android
} // namespace mbgl
