#pragma once

#include "conversion.hpp"

#include <mbgl/util/color.hpp>
#include <jni/jni.hpp>

#include <string>
#include <array>
#include <vector>

namespace mbgl {
namespace android {
namespace conversion {

template <>
struct Converter<jni::jobject*, bool> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const bool& value) const;
};

template <>
struct Converter<jni::jboolean, bool> {
    Result<jni::jboolean> operator()(jni::JNIEnv&, const bool& value) const;
};

template <>
struct Converter<jni::jobject*, float> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const float& value) const;
};

template <>
struct Converter<jni::jfloat, float> {
    Result<jni::jfloat> operator()(jni::JNIEnv&, const float& value) const;
};


template <>
struct Converter<jni::jobject*, double> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const double& value) const;
};

template <>
struct Converter<jni::jdouble, float> {
    Result<jni::jdouble> operator()(jni::JNIEnv&, const double& value) const;
};

/**
 * All integrals. java is limited to 64 bit signed, so...
 * TODO: use BigDecimal for > 64 / unsigned?
 */
template<typename T>
struct Converter<jni::jobject*, T, typename std::enable_if<std::is_integral<T>::value>::type> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const T& value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/Long")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(J)V");
        return {&jni::NewObject(env, *javaClass, *constructor, (jlong) value)};
    }
};

// TODO: convert integral types to primitive jni types

template <>
struct Converter<jni::jobject*, std::string> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const std::string& value) const;
};

template <>
struct Converter<jni::jstring*, std::string> {
    Result<jni::jstring*> operator()(jni::JNIEnv& env, const std::string& value) const;
};

template <>
struct Converter<jni::jobject*, Color> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const Color& value) const;
};

template <std::size_t N>
struct Converter<jni::jobject*, std::array<float, N>> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const std::array<float, N>& value) const {
        std::vector<float> v;
        for (const float& id : value) {
            v.push_back(id);
        }
        return convert<jni::jobject*, std::vector<float>>(env, v);
    }
};

template <>
struct Converter<jni::jobject*, std::vector<std::string>> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const std::vector<std::string>& value) const;
};

template <>
struct Converter<jni::jobject*, std::vector<float>> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const std::vector<float>& value) const;
};

// Java -> C++

template <>
struct Converter<std::string, jni::String> {
    Result<std::string> operator()(jni::JNIEnv& env, const jni::String& value) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
