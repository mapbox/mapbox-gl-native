#include "filter.hpp"

namespace mbgl {
namespace android {

jni::Object<Filter> Filter::fromFilter(jni::JNIEnv& env, const mbgl::style::Filter&) {
    static auto constructor = Filter::javaClass.template GetConstructor<>(env);
    return Filter::javaClass.New(env, constructor);
}

void Filter::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    Filter::javaClass = *jni::Class<Filter>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Filter> Filter::javaClass;


} // namespace android
} // namespace mbgl