#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class RectF : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "android/graphics/RectF"; };

    static float getLeft(jni::JNIEnv&, const jni::Object<RectF>&);

    static float getTop(jni::JNIEnv&, const jni::Object<RectF>&);

    static float getRight(jni::JNIEnv&, const jni::Object<RectF>&);

    static float getBottom(jni::JNIEnv&, const jni::Object<RectF>&);

    static void registerNative(jni::JNIEnv&);
};


} // namespace android
} // namespace mbgl