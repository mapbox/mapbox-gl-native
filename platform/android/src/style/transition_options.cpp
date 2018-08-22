#include "transition_options.hpp"

namespace mbgl {
namespace android {

jni::Local<jni::Object<TransitionOptions>> TransitionOptions::fromTransitionOptions(jni::JNIEnv& env, jlong duration, jlong delay) {
    static auto& javaClass = jni::Class<TransitionOptions>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<TransitionOptions> (jlong, jlong)>(env, "fromTransitionOptions");
    return javaClass.Call(env, method, duration, delay);
}

void TransitionOptions::registerNative(jni::JNIEnv& env) {
    jni::Class<TransitionOptions>::Singleton(env);
}

} // namespace android
} // namespace mbgl