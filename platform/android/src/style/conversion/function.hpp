#pragma once

#include <mbgl/style/property_value.hpp>
#include "../../conversion/conversion.hpp"
#include "../../conversion/constant.hpp"
#include "types.hpp"
#include "function.hpp"

#include <jni/jni.hpp>

#include <tuple>
#include <vector>

namespace mbgl {
namespace android {
namespace conversion {

template <class T>
inline jni::jobject* toFunctionStopJavaArray(jni::JNIEnv& env, std::vector<std::pair<float, T>> value) {
    static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/Function$Stop")).release();
    static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(Ljava/lang/Object;Ljava/lang/Object;)V");

    jni::jarray<jni::jobject>& jarray = jni::NewObjectArray(env, value.size(), *javaClass);

    for(size_t i = 0; i < value.size(); i = i + 1) {
        jni::jobject* in = *convert<jni::jobject*, float>(env, value[i].first);
        jni::jobject* out = *convert<jni::jobject*, T>(env, value[i].second);
        jni::SetObjectArrayElement(env, jarray, i, &jni::NewObject(env, *javaClass, *constructor, in, out));
    }

    return &jarray;
}

template <class T>
struct Converter<jni::jobject*, mbgl::style::Function<T>> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::Function<T>& value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/mapboxsdk/style/layers/Function")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "([Lcom/mapbox/mapboxsdk/style/layers/Function$Stop;)V");
        static jni::jmethodID* withBase = &jni::GetMethodID(env, *javaClass, "withBase", "(F)Lcom/mapbox/mapboxsdk/style/layers/Function;");

        //Create object
        jni::jobject* jfunction = &jni::NewObject(env, *javaClass, *constructor, *toFunctionStopJavaArray(env, value.getStops()));

        //Set base
        jni::CallMethod<jni::jobject*>(env, jfunction, *withBase, value.getBase());

        return {jfunction};
    }
};

} // namespace conversion
} // namespace android
} // namespace mbgl