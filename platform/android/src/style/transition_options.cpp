#include "transition_options.hpp"

namespace mbgl {
namespace android {

jni::Local<jni::Object<TransitionOptions>> TransitionOptions::fromTransitionOptions(jni::JNIEnv& env, jlong duration, jlong delay) {
    static auto& javaClass = jni::Class<TransitionOptions>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<TransitionOptions> (jlong, jlong)>(env, "fromTransitionOptions");
    return javaClass.Call(env, method, duration, delay);
}

long TransitionOptions::getDuration(jni::JNIEnv& env, const jni::Object<TransitionOptions>& transitionOptions) {
    static auto& javaClass = jni::Class<TransitionOptions>::Singleton(env);
    static auto field = javaClass.GetField<jlong>(env, "duration");
    return transitionOptions.Get(env, field);
}

long TransitionOptions::getOffset(jni::JNIEnv& env, const jni::Object<TransitionOptions>& transitionOptions) {
    static auto& javaClass = jni::Class<TransitionOptions>::Singleton(env);
    static auto field = javaClass.GetField<jlong>(env, "offset");
    return transitionOptions.Get(env, field);
}

void TransitionOptions::registerNative(jni::JNIEnv& env) {
    jni::Class<TransitionOptions>::Singleton(env);
}

} // namespace android
} // namespace mbgl