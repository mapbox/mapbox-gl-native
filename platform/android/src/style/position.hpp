#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class Position : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/light/Position"; };

    static jni::Local<jni::Object<Position>> fromPosition(jni::JNIEnv&, jfloat, jfloat, jfloat);

    static void registerNative(jni::JNIEnv&);

    static float getRadialCoordinate(jni::JNIEnv&, const jni::Object<Position>&);
    static float getAzimuthalAngle(jni::JNIEnv&, const jni::Object<Position>&);
    static float getPolarAngle(jni::JNIEnv&, const jni::Object<Position>&);
};

} // namespace android
} // namespace mbgl