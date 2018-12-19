#include "offline_region_definition.hpp"

#include "../geometry/lat_lng_bounds.hpp"
#include "../geojson/geometry.hpp"

#include <exception>

namespace mbgl {
namespace android {

// OfflineRegionDefinition //

void OfflineRegionDefinition::registerNative(jni::JNIEnv& env) {
    jni::Class<OfflineRegionDefinition>::Singleton(env);
}

mbgl::OfflineRegionDefinition OfflineRegionDefinition::getDefinition(JNIEnv& env,
                                                                     const jni::Object<OfflineRegionDefinition>& jDefinition) {
    if (jDefinition.IsInstanceOf(env, jni::Class<OfflineTilePyramidRegionDefinition>::Singleton(env))) {
        return OfflineTilePyramidRegionDefinition::getDefinition(env, jni::Cast(env, jni::Class<OfflineTilePyramidRegionDefinition>::Singleton(env), jDefinition));
    } else if (jDefinition.IsInstanceOf(env, jni::Class<OfflineGeometryRegionDefinition>::Singleton(env))) {
        return OfflineGeometryRegionDefinition::getDefinition(env, jni::Cast(env, jni::Class<OfflineGeometryRegionDefinition>::Singleton(env), jDefinition));
    }

    throw std::runtime_error("Unknown offline region definition java class");
}

// OfflineTilePyramidRegionDefinition //

jni::Local<jni::Object<OfflineRegionDefinition>> OfflineTilePyramidRegionDefinition::New(jni::JNIEnv& env, const mbgl::OfflineTilePyramidRegionDefinition& definition) {
    static auto& javaClass = jni::Class<OfflineTilePyramidRegionDefinition>::Singleton(env);
    static auto constructor = javaClass.GetConstructor<jni::String, jni::Object<LatLngBounds>, jni::jdouble, jni::jdouble, jni::jfloat, jni::jboolean>(env);

    return javaClass.New(env, constructor,
        jni::Make<jni::String>(env, definition.styleURL),
        LatLngBounds::New(env, definition.bounds),
        definition.minZoom,
        definition.maxZoom,
        definition.pixelRatio,
        jni::jboolean(definition.includeIdeographs));
}

mbgl::OfflineTilePyramidRegionDefinition OfflineTilePyramidRegionDefinition::getDefinition(jni::JNIEnv& env, const jni::Object<OfflineTilePyramidRegionDefinition>& jDefinition) {
    // Field references
    static auto& javaClass = jni::Class<OfflineTilePyramidRegionDefinition>::Singleton(env);
    static auto styleURLF = javaClass.GetField<jni::String>(env, "styleURL");
    static auto boundsF = javaClass.GetField<jni::Object<LatLngBounds>>(env, "bounds");
    static auto minZoomF = javaClass.GetField<jni::jdouble>(env, "minZoom");
    static auto maxZoomF = javaClass.GetField<jni::jdouble>(env, "maxZoom");
    static auto pixelRatioF = javaClass.GetField<jni::jfloat>(env, "pixelRatio");
    static auto includeIdeographsF = javaClass.GetField<jni::jboolean >(env, "includeIdeographs");

    return mbgl::OfflineTilePyramidRegionDefinition(
        jni::Make<std::string>(env, jDefinition.Get(env, styleURLF)),
        LatLngBounds::getLatLngBounds(env, jDefinition.Get(env, boundsF)),
        jDefinition.Get(env, minZoomF),
        jDefinition.Get(env, maxZoomF),
        jDefinition.Get(env, pixelRatioF),
        jDefinition.Get(env, includeIdeographsF)
    );
}

void OfflineTilePyramidRegionDefinition::registerNative(jni::JNIEnv& env) {
    jni::Class<OfflineTilePyramidRegionDefinition>::Singleton(env);
}

// OfflineGeometryRegionDefinition //

jni::Local<jni::Object<OfflineRegionDefinition>> OfflineGeometryRegionDefinition::New(jni::JNIEnv& env, const mbgl::OfflineGeometryRegionDefinition& definition) {
    static auto& javaClass = jni::Class<OfflineGeometryRegionDefinition>::Singleton(env);
    static auto constructor = javaClass.GetConstructor<jni::String, jni::Object<geojson::Geometry>, jni::jdouble, jni::jdouble, jni::jfloat, jni::jboolean>(env);

    return javaClass.New(env, constructor,
        jni::Make<jni::String>(env, definition.styleURL),
        geojson::Geometry::New(env, definition.geometry),
        definition.minZoom,
        definition.maxZoom,
        definition.pixelRatio,
        jni::jboolean(definition.includeIdeographs));
}

mbgl::OfflineGeometryRegionDefinition OfflineGeometryRegionDefinition::getDefinition(jni::JNIEnv& env, const jni::Object<OfflineGeometryRegionDefinition>& jDefinition) {
    // Field references
    static auto& javaClass = jni::Class<OfflineGeometryRegionDefinition>::Singleton(env);
    static auto styleURLF = javaClass.GetField<jni::String>(env, "styleURL");
    static auto geometryF = javaClass.GetField<jni::Object<geojson::Geometry>>(env, "geometry");
    static auto minZoomF = javaClass.GetField<jni::jdouble>(env, "minZoom");
    static auto maxZoomF = javaClass.GetField<jni::jdouble>(env, "maxZoom");
    static auto pixelRatioF = javaClass.GetField<jni::jfloat>(env, "pixelRatio");
    static auto includeIdeographsF = javaClass.GetField<jni::jboolean>(env, "includeIdeographs");

    return mbgl::OfflineGeometryRegionDefinition(
        jni::Make<std::string>(env, jDefinition.Get(env, styleURLF)),
        geojson::Geometry::convert(env, jDefinition.Get(env, geometryF)),
        jDefinition.Get(env, minZoomF),
        jDefinition.Get(env, maxZoomF),
        jDefinition.Get(env, pixelRatioF),
        jDefinition.Get(env, includeIdeographsF)
    );
}

void OfflineGeometryRegionDefinition::registerNative(jni::JNIEnv& env) {
    jni::Class<OfflineGeometryRegionDefinition>::Singleton(env);
}

} // namespace android
} // namespace mbgl
