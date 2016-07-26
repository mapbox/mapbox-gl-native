#pragma once

#include "conversion.hpp"

#include <mbgl/util/optional.hpp>
#include <jni/jni.hpp>

#include <string>
#include <array>
#include <vector>
#include <sstream>

namespace mbgl {
namespace android {
namespace conversion {

template <>
struct Converter<jni::jobject*, bool> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const bool& value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/Boolean")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(Z)V");
        return {&jni::NewObject(env, *javaClass, *constructor, (jboolean) value)};
    }
};

template <>
struct Converter<jni::jobject*, float> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const float& value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/Float")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(F)V");
        return {&jni::NewObject(env, *javaClass, *constructor, (jfloat) value)};
    }
};

template <>
struct Converter<jni::jobject*, std::string> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const std::string& value) const {
        return {jni::Make<jni::String>(env, value).Get()};
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
        static jni::jclass* stringCass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/String")).release();
        jni::jarray<jni::jobject>& jarray = jni::NewObjectArray(env, value.size(), *stringCass);

        for(size_t i = 0; i < value.size(); i = i + 1) {
            Result<jni::jobject*> converted = convert<jni::jobject*, std::string>(env, value.at(i));
            jni::SetObjectArrayElement(env, jarray, i, *converted);
        }

        return &jarray;
    }
};

template <>
struct Converter<jni::jobject*, std::vector<float>> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const std::vector<float>& value) const {
        static jni::jclass* floatClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/Float")).release();
        jni::jarray<jni::jobject>& jarray = jni::NewObjectArray(env, value.size(), *floatClass);

        for(size_t i = 0; i < value.size(); i = i + 1) {
            Result<jni::jobject*> converted = convert<jni::jobject*, float>(env, value.at(i));
            jni::SetObjectArrayElement(env, jarray, i, *converted);
        }

        return &jarray;
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
