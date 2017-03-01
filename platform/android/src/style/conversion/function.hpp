#pragma once

#include <mbgl/style/property_value.hpp>
#include "../../conversion/conversion.hpp"
#include "../../conversion/constant.hpp"
#include "types.hpp"
#include "../../java/lang.hpp"
#include "../functions/stop.hpp"
#include "../functions/categorical_stops.hpp"
#include "../functions/exponential_stops.hpp"
#include "../functions/identity_stops.hpp"
#include "../functions/interval_stops.hpp"

#include <jni/jni.hpp>

#include <tuple>
#include <map>

namespace mbgl {
namespace android {
namespace conversion {

/**
 * Conversion from core composite value to java type
 */
class CategoricalValueEvaluator {
public:

    CategoricalValueEvaluator(jni::JNIEnv& _env) : env(_env) {}

    template <class T>
    jni::jobject* operator()(const T &value) const {
        return *convert<jni::jobject*, T>(env, value);
    }

private:
    jni::JNIEnv& env;
};

/**
 * Conversion from core composite value to java type
 */
template <>
struct Converter<jni::jobject*, mbgl::style::CategoricalValue> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::CategoricalValue& value) const {
        CategoricalValueEvaluator evaluator(env);
        return apply_visitor(evaluator, value);
    }
};

template <class I, class O>
jni::Array<jni::Object<Stop>> toFunctionStopJavaArray(jni::JNIEnv& env, std::map<I, O> value) {

    auto jarray = jni::Array<jni::Object<Stop>>::New(env, value.size(), Stop::javaClass);

    size_t i = 0;
    for (auto const& stop : value) {
        jni::jobject* in = *convert<jni::jobject*, I>(env, stop.first);
        jni::jobject* out = *convert<jni::jobject*, O>(env, stop.second);

        auto jstop = Stop::New(env, jni::Object<>(in), jni::Object<>(out));
        jarray.Set(env, i, jstop);

        jni::DeleteLocalRef(env, in);
        jni::DeleteLocalRef(env, out);
        jni::DeleteLocalRef(env, jstop);

        i++;
    }

    return jarray;
}

template <class I, class O>
jni::Array<jni::Object<Stop>> toFunctionStopJavaArray(jni::JNIEnv& env, std::map<float, std::map<I, O>> value) {

    auto jarray = jni::Array<jni::Object<Stop>>::New(env, value.size(), Stop::javaClass);

    for (auto const& zoomLevelMap : value) {
        size_t i = 0;
        for (auto const& stop: zoomLevelMap.second) {
            auto zoom = jni::Object<java::lang::Number>(*convert<jni::jobject*>(env, zoomLevelMap.first));
            auto in = jni::Object<>(*convert<jni::jobject*, I>(env, stop.first));
            auto out = jni::Object<>(*convert<jni::jobject*, O>(env, stop.second));
            auto compositeValue = Stop::CompositeValue::New(env, zoom, in);

            auto jstop = Stop::New(env, compositeValue, out);
            jarray.Set(env, i, jstop);

            jni::DeleteLocalRef(env, zoom);
            jni::DeleteLocalRef(env, in);
            jni::DeleteLocalRef(env, out);
            jni::DeleteLocalRef(env, compositeValue);
            jni::DeleteLocalRef(env, jstop);

            i++;
        }
    }

    return jarray;
}

template <class I, typename O>
inline jni::jobject* convertCompositeStopsArray(jni::JNIEnv& env, std::map<float, std::map<I, O>> value) {
    static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/mapboxsdk/style/functions/stops/Stop")).release();
    static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(Ljava/lang/Object;Ljava/lang/Object;)V");

    jni::jarray<jni::jobject>& jarray = jni::NewObjectArray(env, value.size(), *javaClass);

    size_t i = 0;
    for (auto const& stop : value) {
        jni::jobject* in = *convert<jni::jobject*, I>(env, stop.first);
        jni::jobject* out = *convert<jni::jobject*, O>(env, stop.second);
        jni::SetObjectArrayElement(env, jarray, i, &jni::NewObject(env, *javaClass, *constructor, in, out));
        i++;
        jni::DeleteLocalRef(env, in);
        jni::DeleteLocalRef(env, out);
    }

    return &jarray;
}

/**
 * Conversion from core function stops to Stops java subclasses
 */
template <class T>
class StopsEvaluator {
public:

    StopsEvaluator(jni::JNIEnv& _env) : env(_env) {}

    jni::jobject* operator()(const mbgl::style::CategoricalStops<T> &value) const {
        return CategoricalStops::New(env, toFunctionStopJavaArray(env, value.stops)).Get();
    }

    jni::jobject* operator()(const mbgl::style::CompositeCategoricalStops<T> &value) const {
        return CategoricalStops::New(env, toFunctionStopJavaArray(env, value.stops)).Get();
    }

    jni::jobject* operator()(const mbgl::style::ExponentialStops<T> &value) const {
        return ExponentialStops::New(env, jni::Object<java::lang::Float>(*convert<jni::jobject*>(env, value.base)), toFunctionStopJavaArray(env, value.stops)).Get();
    }

    jni::jobject* operator()(const mbgl::style::CompositeExponentialStops<T> &value) const {
        return ExponentialStops::New(env, jni::Object<java::lang::Float>(*convert<jni::jobject*>(env, value.base)), toFunctionStopJavaArray(env, value.stops)).Get();
    }

    jni::jobject* operator()(const mbgl::style::IdentityStops<T> &) const {
        return IdentityStops::New(env).Get();
    }

    jni::jobject* operator()(const mbgl::style::IntervalStops<T> &value) const {
        return IntervalStops::New(env, toFunctionStopJavaArray(env, value.stops)).Get();
    }

    jni::jobject* operator()(const mbgl::style::CompositeIntervalStops<T> &value) const {
        return IntervalStops::New(env, toFunctionStopJavaArray(env, value.stops)).Get();
    }

private:
    jni::JNIEnv& env;
};

template <class T>
struct Converter<jni::jobject*, mbgl::style::CameraFunction<T>> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::CameraFunction<T>& value) const {
        static jni::jclass* clazz = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/mapboxsdk/style/functions/CameraFunction")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *clazz, "<init>", "(Lcom/mapbox/mapboxsdk/style/functions/stops/Stops;)V");

        StopsEvaluator<T> evaluator(env);
        jni::jobject* stops = apply_visitor(evaluator, value.stops);
        jni::jobject* converted = &jni::NewObject(env, *clazz, *constructor, stops);

        return { converted };
    }
};

template <class T>
struct Converter<jni::jobject*, mbgl::style::SourceFunction<T>> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::SourceFunction<T>& value) const {
        static jni::jclass* clazz = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/mapboxsdk/style/functions/SourceFunction")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *clazz, "<init>",
                                                               "(Ljava/lang/Object;Ljava/lang/String;Lcom/mapbox/mapboxsdk/style/functions/stops/Stops;)V");

        // Convert stops
        StopsEvaluator<T> evaluator(env);
        jni::jobject* stops = apply_visitor(evaluator, value.stops);

        // Convert default value
        jni::jobject* defaultValue = nullptr;
        if (value.defaultValue) {
            defaultValue = *convert<jni::jobject*>(env, *value.defaultValue);
        }

        return { &jni::NewObject(env, *clazz, *constructor, defaultValue, jni::Make<jni::String>(env, value.property).Get(), stops) };
    }
};

template <class T>
struct Converter<jni::jobject*, mbgl::style::CompositeFunction<T>> {

    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::style::CompositeFunction<T>& value) const {
        static jni::jclass* clazz = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/mapboxsdk/style/functions/CompositeFunction")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *clazz, "<init>",
                                                                "(Ljava/lang/Object;Ljava/lang/String;Lcom/mapbox/mapboxsdk/style/functions/stops/Stops;)V");

        // Convert stops
        StopsEvaluator<T> evaluator(env);
        jni::jobject* stops = apply_visitor(evaluator, value.stops);


        // Convert default value
        jni::jobject* defaultValue = nullptr;
        if (value.defaultValue) {
            defaultValue = *convert<jni::jobject*>(env, *value.defaultValue);
        }

        return { &jni::NewObject(env, *clazz, *constructor, defaultValue, jni::Make<jni::String>(env, value.property).Get(), stops) };
    }
};

} // namespace conversion
} // namespace android
} // namespace mbgl
