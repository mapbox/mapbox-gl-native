#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class RectF : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "android/graphics/RectF"; };

    static float getLeft(jni::JNIEnv&, jni::Object<RectF>);

    static float getTop(jni::JNIEnv&, jni::Object<RectF>);

    static float getRight(jni::JNIEnv&, jni::Object<RectF>);

    static float getBottom(jni::JNIEnv&, jni::Object<RectF>);

    static jni::Class<RectF> javaClass;

    static void registerNative(jni::JNIEnv&);

};


} // namespace android
} // namespace mbgl