#pragma once

#include <mbgl/storage/offline.hpp>
#include <jni/jni.hpp>

#include "../file_source.hpp"

#include <memory>

namespace mbgl {
namespace android {

class OfflineRegion {
public:
    class OfflineRegionObserver {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionObserver"; };

        static jni::Class<OfflineRegionObserver> javaClass;

        static void registerNative(jni::JNIEnv&);
    };

    class OfflineRegionStatusCallback {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionStatusCallback"; };

        static void onError(jni::JNIEnv&, jni::Object<OfflineRegionStatusCallback>, std::exception_ptr);

        static void onStatus(jni::JNIEnv&,
                            jni::Object<OfflineRegionStatusCallback>,
                            mbgl::optional<mbgl::OfflineRegionStatus>);

        static jni::Class<OfflineRegionStatusCallback> javaClass;

        static void registerNative(jni::JNIEnv&);
    };

    class OfflineRegionDeleteCallback {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionDeleteCallback"; };

        static void onError(jni::JNIEnv&, jni::Object<OfflineRegionDeleteCallback>, std::exception_ptr);

        static void onDelete(jni::JNIEnv&, jni::Object<OfflineRegionDeleteCallback>);

        static jni::Class<OfflineRegionDeleteCallback> javaClass;

        static void registerNative(jni::JNIEnv&);
    };

    class OfflineRegionUpdateMetadataCallback {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionUpdateMetadataCallback"; };

        static void onError(jni::JNIEnv&, jni::Object<OfflineRegionUpdateMetadataCallback>, std::exception_ptr);

        static void onUpdate(jni::JNIEnv&,
                            jni::Object<OfflineRegionUpdateMetadataCallback>,
                            mbgl::optional<mbgl::OfflineRegionMetadata>);

        static jni::Class<OfflineRegionUpdateMetadataCallback> javaClass;

        static void registerNative(jni::JNIEnv&);
    };

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegion"; };

    OfflineRegion(jni::JNIEnv&, jni::jlong, jni::Object<FileSource>);

    ~OfflineRegion();

    void setOfflineRegionObserver(jni::JNIEnv&, jni::Object<OfflineRegion::OfflineRegionObserver>);

    void setOfflineRegionDownloadState(jni::JNIEnv&, jni::jint);

    void getOfflineRegionStatus(jni::JNIEnv&, jni::Object<OfflineRegion::OfflineRegionStatusCallback>);

    void deleteOfflineRegion(jni::JNIEnv&, jni::Object<OfflineRegionDeleteCallback>);

    void updateOfflineRegionMetadata(jni::JNIEnv&, jni::Array<jni::jbyte>, jni::Object<OfflineRegionUpdateMetadataCallback>);

    static jni::Object<OfflineRegion> New(jni::JNIEnv&, jni::Object<FileSource>, mbgl::OfflineRegion);

    static jni::Array<jni::jbyte> metadata(jni::JNIEnv&, mbgl::OfflineRegionMetadata);

    static mbgl::OfflineRegionMetadata metadata(jni::JNIEnv&, jni::Array<jni::jbyte>);

    static jni::Class<OfflineRegion> javaClass;

    static void registerNative(jni::JNIEnv&);

private:
    std::unique_ptr<mbgl::OfflineRegion> region;
    mbgl::DefaultFileSource& fileSource;
};

} // namespace android
} // namespace mbgl
