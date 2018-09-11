#pragma once

#include <mbgl/storage/offline.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class OfflineRegionDefinition {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegionDefinition"; };

    static void registerNative(jni::JNIEnv&);

    static mbgl::OfflineRegionDefinition getDefinition(JNIEnv& env, const jni::Object<OfflineRegionDefinition>& jDefinition);
};

class OfflineTilePyramidRegionDefinition {
public:
    using SuperTag = OfflineRegionDefinition;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineTilePyramidRegionDefinition"; };

    static jni::Local<jni::Object<OfflineRegionDefinition>> New(jni::JNIEnv&, const mbgl::OfflineTilePyramidRegionDefinition&);

    static mbgl::OfflineTilePyramidRegionDefinition getDefinition(jni::JNIEnv&, const jni::Object<OfflineTilePyramidRegionDefinition>&);

    static void registerNative(jni::JNIEnv&);
};

class OfflineGeometryRegionDefinition {
public:
    using SuperTag = OfflineRegionDefinition;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineGeometryRegionDefinition"; };

    static jni::Local<jni::Object<OfflineRegionDefinition>> New(jni::JNIEnv&, const mbgl::OfflineGeometryRegionDefinition&);

    static mbgl::OfflineGeometryRegionDefinition getDefinition(jni::JNIEnv&, const jni::Object<OfflineGeometryRegionDefinition>&);

    static void registerNative(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
