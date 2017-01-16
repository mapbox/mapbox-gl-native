#pragma once

#include <jni/jni.hpp>

#include <memory>

namespace mbgl {

class DefaultFileSource;

namespace android {

class DefaultFileSourcePeer {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/storage/DefaultFileSource"; }
    static jni::Class<DefaultFileSourcePeer> javaClass;
    static void registerNative(jni::JNIEnv&);

    DefaultFileSourcePeer(jni::JNIEnv&,
                          jni::String cachePath,
                          jni::String assetRoot,
                          jni::jlong maximumCacheSize);
    ~DefaultFileSourcePeer();

    void setAPIBaseURL(jni::JNIEnv&, jni::String);
    jni::String getAPIBaseURL(jni::JNIEnv&);

    void setAccessToken(jni::JNIEnv&, jni::String);
    jni::String getAccessToken(jni::JNIEnv&);

    DefaultFileSource& getFileSource() {
      return *native;
    }

private:
    const std::unique_ptr<DefaultFileSource> native;
};

} // namespace android
} // namespace mbgl
