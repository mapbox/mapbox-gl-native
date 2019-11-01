#include "mapbox.hpp"

namespace mbgl {
namespace android {

jni::Local<jni::Object<AssetManager>> Mapbox::getAssetManager(jni::JNIEnv& env) {
    static auto& javaClass = jni::Class<Mapbox>::Singleton(env);
    auto method = javaClass.GetStaticMethod<jni::Object<AssetManager>()>(env, "getAssetManager");
    return javaClass.Call(env, method);
}

jboolean Mapbox::hasInstance(jni::JNIEnv& env) {
    static auto& javaClass = jni::Class<Mapbox>::Singleton(env);
    auto method = javaClass.GetStaticMethod<jboolean()>(env, "hasInstance");
    return javaClass.Call(env, method);
}

void Mapbox::registerNative(jni::JNIEnv& env) {
    jni::Class<Mapbox>::Singleton(env);
}

} // namespace android
} // namespace mbgl
