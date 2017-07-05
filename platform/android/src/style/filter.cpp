#include "filter.hpp"

namespace mbgl {
namespace android {

jni::Object<Filter::Statement> Filter::fromFilter(jni::JNIEnv& env, const mbgl::style::Filter&) {
    static auto constructor = Filter::Statement::javaClass.template GetConstructor<jni::String, jni::String, jni::Array<jni::Object<>>>(env);
    const char *const equals = "==";
    auto joper = jni::Make<jni::String>(env, equals);
    const char *const key = "type";
    auto jkey = jni::Make<jni::String>(env, key);
    auto jvalues = jni::Array<jni::Object<>>::New(env, 1);
    return Filter::Statement::javaClass.New(env, constructor, joper, jkey, jvalues);
}

jni::Class<Filter> Filter::javaClass;

jni::Class<Filter::Statement> Filter::Statement::javaClass;

void Filter::registerNative(jni::JNIEnv& env) {
    Filter::javaClass = *jni::Class<Filter>::Find(env).NewGlobalRef(env).release();
    Filter::Statement::javaClass = *jni::Class<Filter::Statement>::Find(env).NewGlobalRef(env).release();
}

} // namespace android
} // namespace mbgl