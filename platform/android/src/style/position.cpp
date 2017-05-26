#include "position.hpp"

namespace mbgl {
namespace android {

jni::Object<Position> Position::fromPosition(jni::JNIEnv& env, jfloat radialCoordinate, jfloat azimuthalAngle, jfloat polarAngle) {
    static auto method = Position::javaClass.GetStaticMethod<jni::Object<Position> (jfloat, jfloat, jfloat)>(env, "fromPosition");
    return Position::javaClass.Call(env, method, radialCoordinate, azimuthalAngle, polarAngle);
}

void Position::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    Position::javaClass = *jni::Class<Position>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Position> Position::javaClass;

float Position::getRadialCoordinate(jni::JNIEnv& env, jni::Object<Position> position){
    static auto field = Position::javaClass.GetField<jfloat>(env, "radialCoordinate");
    return position.Get(env, field);
}

float Position::getAzimuthalAngle(jni::JNIEnv& env, jni::Object<Position> position){
    static auto field = Position::javaClass.GetField<jfloat>(env, "azimuthalAngle");
    return position.Get(env, field);
}

float Position::getPolarAngle(jni::JNIEnv& env, jni::Object<Position> position){
    static auto field = Position::javaClass.GetField<jfloat>(env, "polarAngle");
    return position.Get(env, field);
}

} // namespace andr[oid
} // namespace mbgl