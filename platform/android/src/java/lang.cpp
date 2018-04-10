#include "lang.hpp"

namespace mbgl {
namespace android {
namespace java {
namespace lang {

// Float

jni::Object<Float> Float::valueOf(JNIEnv &env, jfloat value) {
    static auto method = javaClass.GetStaticMethod<jni::Object<Float> (jni::jfloat)>(env, "valueOf");
    return javaClass.Call(env, method, value);
}

void Float::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<Float>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Float> Float::javaClass;

// Long

jni::Object<Long> Long::valueOf(JNIEnv &env, jlong value) {
    static auto method = javaClass.GetStaticMethod<jni::Object<Long> (jni::jlong)>(env, "valueOf");
    return javaClass.Call(env, method, value);
}

void Long::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<Long>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Long> Long::javaClass;

// Double

jni::Object<Double> Double::valueOf(JNIEnv &env, jdouble value) {
    static auto method = javaClass.GetStaticMethod<jni::Object<Double> (jni::jdouble)>(env, "valueOf");
    return javaClass.Call(env, method, value);
}

void Double::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<Double>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Double> Double::javaClass;

// Boolean

jni::Object<Boolean> Boolean::valueOf(JNIEnv &env, jboolean value) {
    static auto method = javaClass.GetStaticMethod<jni::Object<Boolean> (jni::jboolean)>(env, "valueOf");
    return javaClass.Call(env, method, value);
}

void Boolean::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<Boolean>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Boolean> Boolean::javaClass;

// Number

void Number::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<Number>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Number> Number::javaClass;

} // namespace lang
} // namespace java
} // namespace android
} // namespace mbgl
