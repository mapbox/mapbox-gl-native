#include "rectf.hpp"

namespace mbgl {
namespace android {

float RectF::getLeft(jni::JNIEnv& env, const jni::Object<RectF>& rectf) {
    static auto& javaClass = jni::Class<RectF>::Singleton(env);
    static auto field = javaClass.GetField<float>(env, "left");
    return rectf.Get(env, field);
}

float RectF::getTop(jni::JNIEnv& env, const jni::Object<RectF>& rectf) {
    static auto& javaClass = jni::Class<RectF>::Singleton(env);
    static auto field = javaClass.GetField<float>(env, "top");
    return rectf.Get(env, field);
}

float RectF::getRight(jni::JNIEnv& env, const jni::Object<RectF>& rectf) {
    static auto& javaClass = jni::Class<RectF>::Singleton(env);
    static auto field = javaClass.GetField<float>(env, "right");
    return rectf.Get(env, field);
}

float RectF::getBottom(jni::JNIEnv& env, const jni::Object<RectF>& rectf) {
    static auto& javaClass = jni::Class<RectF>::Singleton(env);
    static auto field = javaClass.GetField<float>(env, "bottom");
    return rectf.Get(env, field);
}

void RectF::registerNative(jni::JNIEnv& env) {
    jni::Class<RectF>::Singleton(env);
}

} // namespace android
} // namespace mbgl