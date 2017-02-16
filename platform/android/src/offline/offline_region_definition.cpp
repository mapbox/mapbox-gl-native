#include "offline_region_definition.hpp"

#include "../geometry/lat_lng_bounds.hpp"

namespace mbgl {
namespace android {

// OfflineRegionDefinition //

jni::Class<OfflineRegionDefinition> OfflineRegionDefinition::javaClass;

void OfflineRegionDefinition::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<OfflineRegionDefinition>::Find(env).NewGlobalRef(env).release();
}

// OfflineTilePyramidRegionDefinition //

jni::Object<OfflineTilePyramidRegionDefinition> OfflineTilePyramidRegionDefinition::New(jni::JNIEnv& env, mbgl::OfflineTilePyramidRegionDefinition definition) {

    //Convert objects
    auto styleURL = jni::Make<jni::String>(env, definition.styleURL);
    auto bounds = LatLngBounds::New(env, definition.bounds);

    static auto constructor = javaClass.GetConstructor<jni::String, jni::Object<LatLngBounds>, jni::jdouble, jni::jdouble, jni::jfloat>(env);
    auto jdefinition = javaClass.New(env, constructor, styleURL, bounds, definition.minZoom, definition.maxZoom, definition.pixelRatio);

    //Delete References
    jni::DeleteLocalRef(env, styleURL);
    jni::DeleteLocalRef(env, bounds);

    return jdefinition;
}

mbgl::OfflineTilePyramidRegionDefinition OfflineTilePyramidRegionDefinition::getDefinition(jni::JNIEnv& env, jni::Object<OfflineTilePyramidRegionDefinition> jDefinition) {
    // Field references
    static auto styleURLF = javaClass.GetField<jni::String>(env, "styleURL");
    static auto boundsF = javaClass.GetField<jni::Object<LatLngBounds>>(env, "bounds");
    static auto minZoomF = javaClass.GetField<jni::jdouble>(env, "minZoom");
    static auto maxZoomF = javaClass.GetField<jni::jdouble>(env, "maxZoom");
    static auto pixelRatioF = javaClass.GetField<jni::jfloat>(env, "pixelRatio");

    // Get objects
    auto jStyleURL = jDefinition.Get(env, styleURLF);
    auto jBounds = jDefinition.Get(env, boundsF);

    // Create definition
    mbgl::OfflineTilePyramidRegionDefinition definition(
        jni::Make<std::string>(env, jStyleURL),
        LatLngBounds::getLatLngBounds(env, jBounds),
        jDefinition.Get(env, minZoomF),
        jDefinition.Get(env, maxZoomF),
        jDefinition.Get(env, pixelRatioF)
    );

    // Delete references
    jni::DeleteLocalRef(env, jStyleURL);
    jni::DeleteLocalRef(env, jBounds);

    return definition;
}

jni::Class<OfflineTilePyramidRegionDefinition> OfflineTilePyramidRegionDefinition::javaClass;

void OfflineTilePyramidRegionDefinition::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<OfflineTilePyramidRegionDefinition>::Find(env).NewGlobalRef(env).release();
}

} // namespace android
} // namespace mbgl
