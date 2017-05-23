#include "filter.hpp"

namespace mbgl {
namespace android {

jni::Object<Filter::Statement> Filter::fromFilter(jni::JNIEnv& env, const mbgl::style::Filter&) {
    static auto constructor = Filter::Statement::javaClass.template GetConstructor<>(env);
    return Filter::Statement::javaClass.New(env, constructor);
}

jni::Class<Filter> Filter::javaClass;

jni::Class<Filter::Statement> Filter::Statement::javaClass;

void Filter::registerNative(jni::JNIEnv& env) {
    Filter::javaClass = *jni::Class<Filter>::Find(env).NewGlobalRef(env).release();
    Filter::Statement::javaClass = *jni::Class<Filter::Statement>::Find(env).NewGlobalRef(env).release();
}

} // namespace android
} // namespace mbgl