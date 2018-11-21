#include "position.hpp"

namespace mbgl {
namespace android {

jni::Local<jni::Object<Position>> Position::fromPosition(jni::JNIEnv& env, jfloat radialCoordinate, jfloat azimuthalAngle, jfloat polarAngle) {
    static auto& javaClass = jni::Class<Position>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<Position> (jfloat, jfloat, jfloat)>(env, "fromPosition");
    return javaClass.Call(env, method, radialCoordinate, azimuthalAngle, polarAngle);
}

void Position::registerNative(jni::JNIEnv& env) {
    jni::Class<Position>::Singleton(env);
}

float Position::getRadialCoordinate(jni::JNIEnv& env, const jni::Object<Position>& position) {
    static auto& javaClass = jni::Class<Position>::Singleton(env);
    static auto field = javaClass.GetField<jfloat>(env, "radialCoordinate");
    return position.Get(env, field);
}

float Position::getAzimuthalAngle(jni::JNIEnv& env, const jni::Object<Position>& position) {
    static auto& javaClass = jni::Class<Position>::Singleton(env);
    static auto field = javaClass.GetField<jfloat>(env, "azimuthalAngle");
    return position.Get(env, field);
}

float Position::getPolarAngle(jni::JNIEnv& env, const jni::Object<Position>& position) {
    static auto& javaClass = jni::Class<Position>::Singleton(env);
    static auto field = javaClass.GetField<jfloat>(env, "polarAngle");
    return position.Get(env, field);
}

} // namespace android
} // namespace mbgl