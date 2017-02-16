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

private:
    mbgl::DefaultFileSource& fileSource;
};

} // namespace android
} // namespace mbgl
