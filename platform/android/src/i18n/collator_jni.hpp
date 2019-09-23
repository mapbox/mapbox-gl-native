#pragma once

#include <jni/jni.hpp>

/*
    android::Collator and android::Locale are
    the JNI wrappers of
    java/text/Collator and java/util/Locale

    mbgl::Collator is the portable interface
    Both implementations are in collator.cpp
 */

namespace mbgl {
namespace android {

class Locale {
public:
    static constexpr auto Name() { return "java/util/Locale"; };

    /* Requires API level 21+ in order to support script/variant
    static jni::Object<Locale> forLanguageTag(jni::JNIEnv&, jni::String);
    static jni::String toLanguageTag(jni::JNIEnv&, jni::Object<Locale>);
    */
    static jni::Local<jni::Object<Locale>> getDefault(jni::JNIEnv&);
    static jni::Local<jni::String> getLanguage(jni::JNIEnv&, const jni::Object<Locale>&);
    static jni::Local<jni::String> getCountry(jni::JNIEnv&, const jni::Object<Locale>&);

    static jni::Local<jni::Object<Locale>> New(jni::JNIEnv&, const jni::String&);
    static jni::Local<jni::Object<Locale>> New(jni::JNIEnv&, const jni::String&, const jni::String&);


    static void registerNative(jni::JNIEnv&);
};

class Collator {
public:
    static constexpr auto Name() { return "java/text/Collator"; };

    static jni::Local<jni::Object<Collator>> getInstance(jni::JNIEnv&, const jni::Object<Locale>&);

    static void setStrength(jni::JNIEnv&, const jni::Object<Collator>&, jni::jint);

    static jni::jint compare(jni::JNIEnv&, const jni::Object<Collator>&, const jni::String&, const jni::String&);


    static void registerNative(jni::JNIEnv&);
};


class StringUtils {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/utils/StringUtils"; };

    static jni::Local<jni::String> unaccent(jni::JNIEnv&, const jni::String&);


    static void registerNative(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
