#pragma once

#include <jni/jni.hpp>

#include "collator_jni.hpp"

/*
    android::NumberFormat is the JNI wrapper
    of java/text/NumberFormat.
 */

namespace mbgl {
namespace android {

class NumberFormat {
public:
    static constexpr auto Name() { return "java/text/NumberFormat"; };

    static jni::Local<jni::Object<NumberFormat>> getInstance(jni::JNIEnv&, const jni::Object<Locale>&);
    static jni::Local<jni::Object<NumberFormat>> getCurrencyInstance(jni::JNIEnv&, const jni::Object<Locale>&);
    static jni::Local<jni::String> format(jni::JNIEnv&, const jni::Object<NumberFormat>&, jni::jdouble);
    static void setMinimumFractionDigits(jni::JNIEnv&, const jni::Object<NumberFormat>&, jni::jint);
    static void setMaximumFractionDigits(jni::JNIEnv&, const jni::Object<NumberFormat>&, jni::jint);

    static void registerNative(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
