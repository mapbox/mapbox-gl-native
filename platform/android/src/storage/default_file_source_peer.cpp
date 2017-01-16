#include "default_file_source_peer.hpp"

#include <mbgl/storage/default_file_source.hpp>

namespace mbgl {
namespace android {

jni::Class<DefaultFileSourcePeer> DefaultFileSourcePeer::javaClass;

#define NATIVE_METHOD(MethodPtr, name)                                                             \
    jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

void DefaultFileSourcePeer::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<DefaultFileSourcePeer>::Find(env).NewGlobalRef(env).release();

    jni::RegisterNativePeer<DefaultFileSourcePeer>(
        env, javaClass, "peer",
        std::make_unique<DefaultFileSourcePeer, JNIEnv&, jni::String, jni::String, jni::jlong>,
        "initialize", "finalize",
        NATIVE_METHOD(&DefaultFileSourcePeer::setAPIBaseURL, "setAPIBaseURL"),
        NATIVE_METHOD(&DefaultFileSourcePeer::getAPIBaseURL, "getAPIBaseURL"),
        NATIVE_METHOD(&DefaultFileSourcePeer::setAccessToken, "setAccessToken"),
        NATIVE_METHOD(&DefaultFileSourcePeer::getAccessToken, "getAccessToken"));
}

DefaultFileSourcePeer::DefaultFileSourcePeer(jni::JNIEnv& env,
                                             jni::String cachePath,
                                             jni::String assetRoot,
                                             jni::jlong maximumCacheSize)
    : native(std::make_unique<DefaultFileSource>(jni::Make<std::string>(env, cachePath),
                                                 jni::Make<std::string>(env, assetRoot),
                                                 maximumCacheSize)) {
}

DefaultFileSourcePeer::~DefaultFileSourcePeer() = default;

void DefaultFileSourcePeer::setAPIBaseURL(jni::JNIEnv& env, jni::String baseURL) {
    native->setAPIBaseURL(jni::Make<std::string>(env, baseURL));
}

jni::String DefaultFileSourcePeer::getAPIBaseURL(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, native->getAPIBaseURL());
}

void DefaultFileSourcePeer::setAccessToken(jni::JNIEnv& env, jni::String accessToken) {
    native->setAccessToken(jni::Make<std::string>(env, accessToken));
}

jni::String DefaultFileSourcePeer::getAccessToken(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, native->getAccessToken());
}

} // namespace android
} // namespace mbgl
