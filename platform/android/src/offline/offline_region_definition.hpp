#pragma once

#include <mbgl/storage/offline.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class OfflineRegionDefinition {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegionDefinition"; };

    static jni::Class<OfflineRegionDefinition> javaClass;

    static void registerNative(jni::JNIEnv&);

    static mbgl::OfflineRegionDefinition getDefinition(JNIEnv& env, jni::Object<OfflineRegionDefinition> jDefinition);
};

class OfflineTilePyramidRegionDefinition: public OfflineRegionDefinition {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineTilePyramidRegionDefinition"; };

    static jni::Object<OfflineTilePyramidRegionDefinition> New(jni::JNIEnv&, const mbgl::OfflineTilePyramidRegionDefinition&);

    static mbgl::OfflineTilePyramidRegionDefinition getDefinition(jni::JNIEnv&, jni::Object<OfflineTilePyramidRegionDefinition>);

    static jni::Class<OfflineTilePyramidRegionDefinition> javaClass;

    static void registerNative(jni::JNIEnv&);

};

class OfflineGeometryRegionDefinition: public OfflineRegionDefinition {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineGeometryRegionDefinition"; };

    static jni::Object<OfflineGeometryRegionDefinition> New(jni::JNIEnv&, const mbgl::OfflineGeometryRegionDefinition&);

    static mbgl::OfflineGeometryRegionDefinition getDefinition(jni::JNIEnv&, jni::Object<OfflineGeometryRegionDefinition>);

    static jni::Class<OfflineGeometryRegionDefinition> javaClass;

    static void registerNative(jni::JNIEnv&);

};

} // namespace android
} // namespace mbgl
