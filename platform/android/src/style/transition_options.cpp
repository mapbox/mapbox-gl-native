#include "transition_options.hpp"

namespace mbgl {
namespace android {

jni::Local<jni::Object<TransitionOptions>> TransitionOptions::fromTransitionOptions(jni::JNIEnv& env, jlong duration, jlong delay, jboolean enablePlacementTransitions) {
    static auto& javaClass = jni::Class<TransitionOptions>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<TransitionOptions> (jlong, jlong, jboolean)>(env, "fromTransitionOptions");
    return javaClass.Call(env, method, duration, delay, enablePlacementTransitions);
}

long TransitionOptions::getDuration(jni::JNIEnv& env, const jni::Object<TransitionOptions>& transitionOptions) {
    static auto& javaClass = jni::Class<TransitionOptions>::Singleton(env);
    static auto field = javaClass.GetField<jlong>(env, "duration");
    return transitionOptions.Get(env, field);
}

long TransitionOptions::getDelay(jni::JNIEnv& env, const jni::Object<TransitionOptions>& transitionOptions) {
    static auto& javaClass = jni::Class<TransitionOptions>::Singleton(env);
    static auto field = javaClass.GetField<jlong>(env, "delay");
    return transitionOptions.Get(env, field);
}

bool TransitionOptions::isEnablePlacementTransitions(jni::JNIEnv& env, const jni::Object<mbgl::android::TransitionOptions>& transitionOptions) {
    static auto& javaClass = jni::Class<TransitionOptions>::Singleton(env);
    static auto field = javaClass.GetField<jboolean >(env, "enablePlacementTransitions");
    return transitionOptions.Get(env, field);
}

void TransitionOptions::registerNative(jni::JNIEnv& env) {
    jni::Class<TransitionOptions>::Singleton(env);
}

} // namespace android
} // namespace mbgl