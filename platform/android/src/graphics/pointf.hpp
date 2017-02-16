#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class PointF : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "android/graphics/PointF"; };

    static jni::Object<PointF> New(jni::JNIEnv&, float, float);

    static jni::Class<PointF> javaClass;

    static void registerNative(jni::JNIEnv&);

};


} // namespace android
} // namespace mbgl