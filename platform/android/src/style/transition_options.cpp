#include "transition_options.hpp"

namespace mbgl {
namespace android {

jni::Object<TransitionOptions> TransitionOptions::fromTransitionOptions(jni::JNIEnv& env, jlong duration, jlong delay) {
    static auto method = TransitionOptions::javaClass.GetStaticMethod<jni::Object<TransitionOptions> (jlong, jlong)>(env, "fromTransitionOptions");
    return TransitionOptions::javaClass.Call(env, method, duration, delay);
}

void TransitionOptions::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    TransitionOptions::javaClass = *jni::Class<TransitionOptions>::Find(env).NewGlobalRef(env).release();
}

jni::Class<TransitionOptions> TransitionOptions::javaClass;


} // namespace android
} // namespace mbgl