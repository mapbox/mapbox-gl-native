#include "rectf.hpp"

namespace mbgl {
namespace android {

float RectF::getLeft(jni::JNIEnv& env, jni::Object<RectF> rectf) {
    static auto field = RectF::javaClass.GetField<float>(env, "left");
    return rectf.Get(env, field);
}

float RectF::getTop(jni::JNIEnv& env, jni::Object<RectF> rectf) {
    static auto field = RectF::javaClass.GetField<float>(env, "top");
    return rectf.Get(env, field);
}

float RectF::getRight(jni::JNIEnv& env, jni::Object<RectF> rectf) {
    static auto field = RectF::javaClass.GetField<float>(env, "right");
    return rectf.Get(env, field);
}

float RectF::getBottom(jni::JNIEnv& env, jni::Object<RectF> rectf) {
    static auto field = RectF::javaClass.GetField<float>(env, "bottom");
    return rectf.Get(env, field);
}

void RectF::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    RectF::javaClass = *jni::Class<RectF>::Find(env).NewGlobalRef(env).release();
}

jni::Class<RectF> RectF::javaClass;


} // namespace android
} // namespace mbgl