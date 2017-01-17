#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/default_file_source.hpp>

#include "../jni/peer.hpp"

namespace mbgl {
namespace android {

template <>
constexpr auto Peer<DefaultFileSource>::Name() {
    return "com/mapbox/mapboxsdk/storage/DefaultFileSource";
}

class Impl : public Peer<DefaultFileSource> {
public:
    Impl(jni::JNIEnv& env,
         jni::String cachePath,
         jni::String assetRoot,
         jni::jlong maximumCacheSize)
        : Peer(jni::Make<std::string>(env, cachePath),
               jni::Make<std::string>(env, assetRoot),
               maximumCacheSize) {
    }

    void setAPIBaseURL(jni::JNIEnv& env, jni::String baseURL) {
        native->setAPIBaseURL(jni::Make<std::string>(env, baseURL));
    }

    jni::String getAPIBaseURL(jni::JNIEnv& env) {
        return jni::Make<jni::String>(env, native->getAPIBaseURL());
    }

    void setAccessToken(jni::JNIEnv& env, jni::String accessToken) {
        native->setAccessToken(jni::Make<std::string>(env, accessToken));
    }

    jni::String getAccessToken(jni::JNIEnv& env) {
        return jni::Make<jni::String>(env, native->getAccessToken());
    }
};

#define NATIVE_METHOD(MethodPtr, name)                                                             \
    jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

template <>
void Peer<DefaultFileSource>::RegisterNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<Peer<DefaultFileSource>>::Find(env).NewGlobalRef(env).release();

    jni::RegisterNativePeer<Impl>(
        env, javaClass, peer,
        std::make_unique<Impl, JNIEnv&, jni::String, jni::String, jni::jlong>,
        "initialize", "finalize",
        NATIVE_METHOD(&Impl::setAPIBaseURL, "setAPIBaseURL"),
        NATIVE_METHOD(&Impl::getAPIBaseURL, "getAPIBaseURL"),
        NATIVE_METHOD(&Impl::setAccessToken, "setAccessToken"),
        NATIVE_METHOD(&Impl::getAccessToken, "getAccessToken"));
}

} // namespace android
} // namespace mbgl
