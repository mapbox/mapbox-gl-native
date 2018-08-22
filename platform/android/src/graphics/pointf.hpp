#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class PointF : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "android/graphics/PointF"; };

    static jni::Local<jni::Object<PointF>> New(jni::JNIEnv&, float, float);

    static mbgl::ScreenCoordinate getScreenCoordinate(jni::JNIEnv&, const jni::Object<PointF>&);

    static void registerNative(jni::JNIEnv&);
};


} // namespace android
} // namespace mbgl