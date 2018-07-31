#pragma once


#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/offline.hpp>
#include <jni/jni.hpp>

#include "../file_source.hpp"
#include "offline_region.hpp"
#include "offline_region_definition.hpp"

namespace mbgl {
namespace android {

class OfflineManager {
public:

    class ListOfflineRegionsCallback {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineManager$ListOfflineRegionsCallback";}

        static void onError(jni::JNIEnv&, jni::Object<OfflineManager::ListOfflineRegionsCallback>, std::exception_ptr);

        static void onList(jni::JNIEnv&,
                            jni::Object<FileSource>,
                            jni::Object<OfflineManager::ListOfflineRegionsCallback>,
                            mbgl::optional<std::vector<mbgl::OfflineRegion>>);

        static jni::Class<OfflineManager::ListOfflineRegionsCallback> javaClass;

        static void registerNative(jni::JNIEnv&);
    };

    class CreateOfflineRegionCallback {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineManager$CreateOfflineRegionCallback"; }

        static void onError(jni::JNIEnv&, jni::Object<OfflineManager::CreateOfflineRegionCallback>, std::exception_ptr);

        static void onCreate(jni::JNIEnv&,
                            jni::Object<FileSource>,
                            jni::Object<OfflineManager::CreateOfflineRegionCallback>,
                            mbgl::optional<mbgl::OfflineRegion>);

        static jni::Class<OfflineManager::CreateOfflineRegionCallback> javaClass;

        static void registerNative(jni::JNIEnv&);
    };

    class PutOfflineCallback {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineManager$PutOfflineCallback";}

        static void onError(jni::JNIEnv&, jni::Object<OfflineManager::PutOfflineCallback>, std::exception_ptr);

        static void onPut(jni::JNIEnv&, jni::Object<OfflineManager::PutOfflineCallback>);

        static jni::Class<OfflineManager::PutOfflineCallback> javaClass;

        static void registerNative(jni::JNIEnv&);
    };

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineManager"; };

    static jni::Class<OfflineManager> javaClass;

    static void registerNative(jni::JNIEnv&);

    OfflineManager(jni::JNIEnv&, jni::Object<FileSource>);
    ~OfflineManager();

    void setOfflineMapboxTileCountLimit(jni::JNIEnv&, jni::jlong limit);

    void listOfflineRegions(jni::JNIEnv&, jni::Object<FileSource>, jni::Object<ListOfflineRegionsCallback> callback);

    void createOfflineRegion(jni::JNIEnv&,
                             jni::Object<FileSource> jFileSource_,
                             jni::Object<OfflineRegionDefinition> definition,
                             jni::Array<jni::jbyte> metadata,
                             jni::Object<OfflineManager::CreateOfflineRegionCallback> callback);

    void putResourceWithUrl(jni::JNIEnv&,
                            jni::String url_,
                            jni::Array<jni::jbyte> arr,
                            jboolean compressed,
                            jlong modified,
                            jlong expires,
                            jni::String eTag_,
                            jlong regionId);

    void putTileWithUrlTemplate(jni::JNIEnv&,
                                jni::String urlTemplate_,
                                jfloat pixelRatio,
                                jint x,
                                jint y,
                                jint z,
                                jni::Array<jni::jbyte> arr,
                                jboolean compressed,
                                jlong modified,
                                jlong expires,
                                jni::String eTag_,
                                jlong regionId);

    void commitResourcesForPack(jni::JNIEnv&, jlong regionId, jni::Object<OfflineManager::PutOfflineCallback> callback_);

private:
    mbgl::DefaultFileSource& fileSource;
};

} // namespace android
} // namespace mbgl
