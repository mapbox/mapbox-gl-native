#pragma once

#include "conversion.hpp"

#include "../java/lang.hpp"

#include <mbgl/util/optional.hpp>

#include <string>
#include <array>
#include <vector>
#include <sstream>

namespace mbgl {
namespace android {
namespace conversion {

template <>
struct Converter<jni::jobject*, bool> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, bool value) const {
        return { jni::Make<java::lang::Boolean>(env, value).Get() };
    }
};

template <>
struct Converter<jni::jboolean, bool> {
    Result<jni::jboolean> operator()(jni::JNIEnv&, bool value) const {
        return { (jni::jboolean)value };
    }
};

template <>
struct Converter<jni::jobject*, float> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, float value) const {
        return { jni::Make<java::lang::Float>(env, value).Get() };
    }
};

template <>
struct Converter<jni::jfloat, float> {
    Result<jni::jfloat> operator()(jni::JNIEnv&, float value) const {
        return { (jni::jfloat)value };
    }
};

template <>
struct Converter<jni::jobject*, double> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, double value) const {
        return { jni::Make<java::lang::Double>(env, value).Get() };
    }
};

template <>
struct Converter<jni::jdouble, float> {
    Result<jni::jdouble> operator()(jni::JNIEnv&, double value) const {
        return { (jni::jdouble)value };
    }
};

/**
 * All integrals. java is limited to 64 bit signed, so...
 * TODO: use BigDecimal for > 64 / unsigned?
 */
template <typename T>
struct Converter<jni::jobject*, T, typename std::enable_if<std::is_integral<T>::value>::type> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, T value) const {
        return { jni::Make<java::lang::Long>(env, value).Get() };
    }
};

// TODO: convert integral types to primitive jni types

template <>
struct Converter<jni::jobject*, std::string> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const std::string& value) const {
        return { jni::Make<jni::String>(env, value).Get() };
    }
};

template <>
struct Converter<jni::jstring*, std::string> {
    Result<jni::jstring*> operator()(jni::JNIEnv& env, const std::string& value) const {
        return { jni::Make<jni::String>(env, value).Get() };
    }
};

template <>
struct Converter<jni::jobject*, Color> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const Color& value) const {
        std::stringstream sstream;
        sstream << "rgba(" << value.r << ", " << value.g << ", " << value.b << ", "  << value.a << ")";
        std::string result = sstream.str();
        return convert<jni::jobject*, std::string>(env, result);
    }
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
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const std::vector<std::string>& value) const {
        auto array = jni::Make<jni::Array<java::lang::String>>(env, value.size());
        for (size_t i = 0; i < value.size(); ++i) {
            array.Set(env, i, jni::Make<java::lang::String>(env, value.at(i)));
        }
        return { array.Get() };
    }
};

template <>
struct Converter<jni::jobject*, std::vector<float>> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const std::vector<float>& value) const {
        auto array = jni::Make<jni::Array<java::lang::Float>>(env, value.size());
        for (size_t i = 0; i < value.size(); ++i) {
            array.Set(env, i, jni::Make<java::lang::Float>(env, value.at(i)));
        }
        return { array.Get() };
    }
};

// Java -> C++

template <>
struct Converter<std::string, jni::String> {
    Result<std::string> operator()(jni::JNIEnv& env, const jni::String& value) const {
        return { jni::Make<std::string>(env, value) };
    }
};

} // namespace conversion
} // namespace android
} // namespace mbgl
