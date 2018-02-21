#pragma once

#include "../../conversion/constant.hpp"
#include "../../conversion/collection.hpp"

#include <mapbox/geometry.hpp>
#include <jni/jni.hpp>
#include "../../jni/local_object.hpp"
#include "mapbox/geometry/feature.hpp"

namespace mbgl {
namespace android {
namespace conversion {

/**
 * Turn mapbox::geometry::value type into Java Json
 */
class JsonEvaluator {
public:

    jni::JNIEnv& env;

    jni::jobject* operator()(const mapbox::geometry::null_value_t) const {
       return (jni::jobject*) nullptr;;
    }

    jni::jobject* operator()(const bool& value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/google/gson/JsonPrimitive")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(Ljava/lang/Boolean;)V");

        // Create JsonPrimitive
        jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, *convert<jni::jobject*, bool>(env, value));
        jni::jobject* object = &jni::NewObject(env, *javaClass, *constructor, *converted);

        return object;
    }

    template <class Number>
    jni::jobject* operator()(const Number& value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/google/gson/JsonPrimitive")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(Ljava/lang/Number;)V");

        // Create JsonPrimitive
        jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, *convert<jni::jobject*, Number>(env, value));
        jni::jobject* object = &jni::NewObject(env, *javaClass, *constructor, converted.get());

        return object;
    }

    jni::jobject* operator()(const std::string value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/google/gson/JsonPrimitive")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(Ljava/lang/String;)V");

        // Create JsonPrimitive
        jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, *convert<jni::jobject*, std::string>(env, value));
        jni::jobject* object = &jni::NewObject(env, *javaClass, *constructor, converted.get());

        return object;
    }

    jni::jobject* operator()(const std::vector<mapbox::geometry::value> values) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/google/gson/JsonArray")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "()V");;
        static jni::jmethodID* add = &jni::GetMethodID(env, *javaClass, "add", "(Lcom/google/gson/JsonElement;)V");

        // Create json array
        jni::jobject* jarray = &jni::NewObject(env, *javaClass, *constructor);

        // Add values
        for (const auto &v : values) {
            jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, mapbox::geometry::value::visit(v, *this));
            jni::CallMethod<void>(env, jarray, *add, converted.get());
        }

        return jarray;
    }

    jni::jobject* operator()(const std::unordered_map<std::string, mapbox::geometry::value> value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/google/gson/JsonObject")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "()V");;
        static jni::jmethodID* add = &jni::GetMethodID(env, *javaClass, "add", "(Ljava/lang/String;Lcom/google/gson/JsonElement;)V");

        // Create json object
        jni::jobject* jsonObject = &jni::NewObject(env, *javaClass, *constructor);

        // Add items
        for (auto &item : value) {
            jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, mbgl::Value::visit(item.second, *this));
            jni::LocalObject<jni::jobject> key = jni::NewLocalObject(env, *convert<jni::jobject*, std::string>(env, item.first));
            jni::CallMethod<void>(env, jsonObject, *add, key.get(), converted.get());
        }

        return jsonObject;
    }

private:

};

/**
 * mapbox::geometry::value -> Java Json
 */
template <>
struct Converter<jni::jobject*, mapbox::geometry::value> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mapbox::geometry::value& value) const {
        JsonEvaluator evaluator { env } ;
        jni::jobject* converted = mapbox::geometry::value::visit(value, evaluator);
        return { converted };
    }
};

}
}
}
