#pragma once

#include <mbgl/storage/default_file_source.hpp>

#include "asset_manager.hpp"

#include <jni/jni.hpp>

namespace mbgl {

template <typename T> class Actor;
class ResourceTransform;

namespace android {

/**
 * Peer class for the Android FileSource holder. Ensures that a single DefaultFileSource is used
 */
class FileSource {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/storage/FileSource"; };

    struct ResourceTransformCallback {
        static constexpr auto Name() { return "com/mapbox/mapboxsdk/storage/FileSource$ResourceTransformCallback"; }

        static std::string onURL(jni::JNIEnv&, jni::Object<FileSource::ResourceTransformCallback>, int, std::string);

        static jni::Class<ResourceTransformCallback> javaClass;
    };

    FileSource(jni::JNIEnv&, jni::String, jni::String, jni::Object<AssetManager>);

    ~FileSource();

    jni::String getAccessToken(jni::JNIEnv&);

    void setAccessToken(jni::JNIEnv&, jni::String);

    void setAPIBaseUrl(jni::JNIEnv&, jni::String);

    void setResourceTransform(jni::JNIEnv&, jni::Object<FileSource::ResourceTransformCallback>);

    void resume(jni::JNIEnv&);

    void pause(jni::JNIEnv&);

    jni::jboolean isResumed(jni::JNIEnv&);

    static jni::Class<FileSource> javaClass;

    static FileSource* getNativePeer(jni::JNIEnv&, jni::Object<FileSource>);

    static mbgl::DefaultFileSource& getDefaultFileSource(jni::JNIEnv&, jni::Object<FileSource>);

    static void registerNative(jni::JNIEnv&);

private:
    optional<int> activationCounter;
    std::unique_ptr<Actor<ResourceTransform>> resourceTransform;
    std::unique_ptr<mbgl::DefaultFileSource> fileSource;
};


} // namespace android
} // namespace mbgl
