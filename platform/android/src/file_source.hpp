#pragma once

#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/resource_options.hpp>

#include "asset_manager.hpp"

#include <jni/jni.hpp>

namespace mbgl {

template <typename T> class Actor;
class ResourceTransform;
using mbgl::PathChangeCallback;

namespace android {

/**
 * Peer class for the Android FileSource holder. Ensures that a single DefaultFileSource is used
 */
class FileSource {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/storage/FileSource"; };

    struct ResourceTransformCallback {
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/storage/FileSource$ResourceTransformCallback"; }

        static std::string onURL(jni::JNIEnv&, const jni::Object<FileSource::ResourceTransformCallback>&, int, std::string);
    };

    struct ResourcesCachePathChangeCallback {
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/storage/FileSource$ResourcesCachePathChangeCallback";}

        static void onSuccess(jni::JNIEnv&,
                              const jni::Object<FileSource::ResourcesCachePathChangeCallback>&,
                              const jni::String&);

        static void onError(jni::JNIEnv&,
                              const jni::Object<FileSource::ResourcesCachePathChangeCallback>&,
                              const jni::String&);
    };

    FileSource(jni::JNIEnv&, const jni::String&, const jni::String&);

    ~FileSource();

    jni::Local<jni::String> getAccessToken(jni::JNIEnv&);

    void setAccessToken(jni::JNIEnv&, const jni::String&);

    void setAPIBaseUrl(jni::JNIEnv&, const jni::String&);

    void setResourceTransform(jni::JNIEnv&, const jni::Object<FileSource::ResourceTransformCallback>&);

    void setResourceCachePath(jni::JNIEnv&, const jni::String&, const jni::Object<FileSource::ResourcesCachePathChangeCallback>&);

    void resume(jni::JNIEnv&);

    void pause(jni::JNIEnv&);

    jni::jboolean isResumed(jni::JNIEnv&);

    static FileSource* getNativePeer(jni::JNIEnv&, const jni::Object<FileSource>&);

    static mbgl::ResourceOptions getSharedResourceOptions(jni::JNIEnv&, const jni::Object<FileSource>&);

    static void registerNative(jni::JNIEnv&);

private:
    const std::string DATABASE_FILE = "/mbgl-offline.db";
    optional<int> activationCounter;
    mbgl::ResourceOptions resourceOptions;
    std::unique_ptr<Actor<ResourceTransform>> resourceTransform;
    std::unique_ptr<Actor<PathChangeCallback>> pathChangeCallback;
    std::shared_ptr<mbgl::DefaultFileSource> fileSource;
};


} // namespace android
} // namespace mbgl
