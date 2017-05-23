#include "filter.hpp"

namespace mbgl {
namespace android {

jni::Object<Statement> Filter::fromFilter(jni::JNIEnv& env, const mbgl::style::Filter&) {
    static auto constructor = Statement::javaClass.template GetConstructor<>(env);

    return Statement::javaClass.New(env, constructor);
}

void Filter::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    Filter::javaClass = *jni::Class<Filter>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Filter> Filter::javaClass;


} // namespace android
} // namespace mbgl