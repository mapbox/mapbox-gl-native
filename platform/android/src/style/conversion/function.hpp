#pragma once

#include <mbgl/style/property_value.hpp>
#include "../../conversion/conversion.hpp"
#include "../../conversion/constant.hpp"
#include "types.hpp"

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
inline jni::jobject* toFunctionStopJavaArray(jni::JNIEnv& env, std::map<I, O> value) {
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
        static jni::jclass* clazz = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/mapboxsdk/style/functions/stops/CategoricalStops")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *clazz, "<init>", "([Lcom/mapbox/mapboxsdk/style/functions/stops/Stop;)V");

        return &jni::NewObject(env, *clazz, *constructor, toFunctionStopJavaArray(env, value.stops));
    }

    jni::jobject* operator()(const mbgl::style::ExponentialStops<T> &value) const {
        static jni::jclass* clazz = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/mapboxsdk/style/functions/stops/ExponentialStops")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *clazz, "<init>", "(Ljava/lang/Float;[Lcom/mapbox/mapboxsdk/style/functions/stops/Stop;)V");

        return &jni::NewObject(env, *clazz, *constructor,
            *convert<jni::jobject*>(env, value.base),
            toFunctionStopJavaArray(env, value.stops));
    }

    jni::jobject* operator()(const mbgl::style::IdentityStops<T> &) const {
        static jni::jclass* clazz = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/mapboxsdk/style/functions/stops/IdentityStops")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *clazz, "<init>", "()V");

        return &jni::NewObject(env, *clazz, *constructor);
    }

    jni::jobject* operator()(const mbgl::style::IntervalStops<T> &value) const {
        static jni::jclass* clazz = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/mapboxsdk/style/functions/stops/IntervalStops")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *clazz, "<init>", "([Lcom/mapbox/mapboxsdk/style/functions/stops/Stop;)V");

        return &jni::NewObject(env, *clazz, *constructor, toFunctionStopJavaArray(env, value.stops));
    }

private:
    jni::JNIEnv& env;
};

template <class T, typename X>
inline jni::jobject* convertCompositeStopsArray(jni::JNIEnv& env, std::map<float, T> value) {
    // Create Java Map
    static jni::jclass* mapClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/util/HashMap")).release();
    static jni::jmethodID* mapConstructor = &jni::GetMethodID(env, *mapClass, "<init>", "()V");
    static jni::jmethodID* mapPutMethod = &jni::GetMethodID(env, *mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    jni::jobject& map = jni::NewObject(env, *mapClass, *mapConstructor);

    // Add converted Stops for each zoom value
    StopsEvaluator<X> evaluator(env);
    for (auto const& entry : value) {
        jni::jobject* zoom = *convert<jni::jobject*, float>(env, entry.first);
        jni::jobject* stops = evaluator(entry.second);
        jni::CallMethod<jni::jobject*>(env, &map, *mapPutMethod, zoom, stops);
        jni::DeleteLocalRef(env, zoom);
        jni::DeleteLocalRef(env, stops);
    }

    // Create CompositeStops from Java Map
    static jni::jclass* compositeStopsClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/mapboxsdk/style/functions/stops/CompositeStops")).release();
    static jni::jmethodID* compositeStopsConstructor = &jni::GetMethodID(env, *compositeStopsClass, "<init>", "(Ljava/util/Map;)V");
    jni::jobject& compositeStops = jni::NewObject(env, *compositeStopsClass, *compositeStopsConstructor, &map);
    jni::DeleteLocalRef(env, &map);

    return &compositeStops;
}

/**
 * Conversion from core composite function stops to CompositeFunctionStops java type
 */
template <class T>
class CompositeStopsEvaluator {
public:

    CompositeStopsEvaluator(jni::JNIEnv& _env) : env(_env) {}

    jni::jobject* operator()(const std::map<float, mbgl::style::CategoricalStops<T>> &value) const {
        return convertCompositeStopsArray<mbgl::style::CategoricalStops<T>, T>(env, value);
    }

    jni::jobject* operator()(const std::map<float, mbgl::style::ExponentialStops<T>> &value) const {
        return convertCompositeStopsArray<mbgl::style::ExponentialStops<T>, T>(env, value);
    }

    jni::jobject* operator()(const std::map<float, mbgl::style::IntervalStops<T>> &value) const {
        return convertCompositeStopsArray<mbgl::style::IntervalStops<T>, T>(env, value);
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
                                                                "(Ljava/lang/Object;Ljava/lang/String;Lcom/mapbox/mapboxsdk/style/functions/stops/CompositeStops;)V");

        // Convert stops
        CompositeStopsEvaluator<T> evaluator(env);
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
