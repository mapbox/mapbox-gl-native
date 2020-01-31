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
    };

    class OfflineRegionStatusCallback {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionStatusCallback"; };

        static void onError(jni::JNIEnv&, const jni::Object<OfflineRegionStatusCallback>&, std::exception_ptr);

        static void onStatus(jni::JNIEnv&,
                            const jni::Object<OfflineRegionStatusCallback>&,
                            mbgl::optional<mbgl::OfflineRegionStatus>);
    };

    class OfflineRegionDeleteCallback {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionDeleteCallback"; };

        static void onError(jni::JNIEnv&, const jni::Object<OfflineRegionDeleteCallback>&, std::exception_ptr);

        static void onDelete(jni::JNIEnv&, const jni::Object<OfflineRegionDeleteCallback>&);
    };

    class OfflineRegionInvalidateCallback {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionInvalidateCallback"; };

        static void onError(jni::JNIEnv&, const jni::Object<OfflineRegionInvalidateCallback>&, std::exception_ptr);

        static void onInvalidate(jni::JNIEnv&, const jni::Object<OfflineRegionInvalidateCallback>&);
    };

    class OfflineRegionUpdateMetadataCallback {
    public:
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegion$OfflineRegionUpdateMetadataCallback"; };

        static void onError(jni::JNIEnv&, const jni::Object<OfflineRegionUpdateMetadataCallback>&, std::exception_ptr);

        static void onUpdate(jni::JNIEnv&,
                            const jni::Object<OfflineRegionUpdateMetadataCallback>&,
                            mbgl::optional<mbgl::OfflineRegionMetadata>);
    };

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegion"; };

    OfflineRegion(jni::JNIEnv&, jni::jlong, const jni::Object<FileSource>&);

    ~OfflineRegion();

    void setOfflineRegionObserver(jni::JNIEnv&, const jni::Object<OfflineRegion::OfflineRegionObserver>&);

    void setOfflineRegionDownloadState(jni::JNIEnv&, jni::jint);

    void getOfflineRegionStatus(jni::JNIEnv&, const jni::Object<OfflineRegion::OfflineRegionStatusCallback>&);

    void deleteOfflineRegion(jni::JNIEnv&, const jni::Object<OfflineRegionDeleteCallback>&);

    void invalidateOfflineRegion(jni::JNIEnv&, const jni::Object<OfflineRegionInvalidateCallback>&);

    void updateOfflineRegionMetadata(jni::JNIEnv&, const jni::Array<jni::jbyte>&, const jni::Object<OfflineRegionUpdateMetadataCallback>&);

    static jni::Local<jni::Object<OfflineRegion>> New(jni::JNIEnv&, const jni::Object<FileSource>&, mbgl::OfflineRegion);

    static jni::Local<jni::Array<jni::jbyte>> metadata(jni::JNIEnv&, mbgl::OfflineRegionMetadata);

    static mbgl::OfflineRegionMetadata metadata(jni::JNIEnv&, const jni::Array<jni::jbyte>&);

    static void registerNative(jni::JNIEnv&);

private:
    std::unique_ptr<mbgl::OfflineRegion> region;
    std::shared_ptr<mbgl::DefaultFileSource> fileSource;
};

} // namespace android
} // namespace mbgl
