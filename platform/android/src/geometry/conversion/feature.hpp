#pragma once

#include "../../conversion/constant.hpp"
#include "../../conversion/conversion.hpp"
#include "geometry.hpp"

#include <mbgl/util/feature.hpp>
#include <mapbox/variant.hpp>
#include <mapbox/geometry.hpp>

#include <jni/jni.hpp>
#include "../../jni/local_object.hpp"

#include <string>
#include <array>
#include <vector>
#include <sstream>

#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace android {
namespace conversion {

/**
 * Turn feature identifier into std::string
 */
class FeatureIdVisitor {
public:

    template<class T>
    std::string operator()(const T& i) const {
        return std::to_string(i);
    }

    std::string operator()(const std::string& i) const {
        return i;
    }

    std::string operator()(const std::nullptr_t&) const {
        return "";
    }

};

/**
 * Turn properties into Java GSON JsonObject's
 */
class PropertyValueEvaluator {
public:
    jni::JNIEnv& env;

    /**
     * null
     */
    jni::jobject* operator()(const mapbox::geometry::null_value_t &) const {
        return (jni::jobject*) nullptr;
    }

    /**
     * Boolean primitive
     */
    jni::jobject* operator()(const bool& value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/google/gson/JsonPrimitive")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(Ljava/lang/Boolean;)V");

        //Create JsonPrimitive
        jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, *convert<jni::jobject*, bool>(env, value));
        jni::jobject* object = &jni::NewObject(env, *javaClass, *constructor, *converted);

        return object;
    }

    /**
     * String primitive
     */
    jni::jobject* operator()(const std::string& value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/google/gson/JsonPrimitive")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(Ljava/lang/String;)V");

        //Create JsonPrimitive
        jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, *convert<jni::jobject*, std::string>(env, value));
        jni::jobject* object = &jni::NewObject(env, *javaClass, *constructor, converted.get());

        return object;
    }

    /**
     * Number primitives
     */
    template <class Number>
    jni::jobject* operator()(const Number& value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/google/gson/JsonPrimitive")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(Ljava/lang/Number;)V");

        //Create JsonPrimitive
        jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, *convert<jni::jobject*, Number>(env, value));
        jni::jobject* object = &jni::NewObject(env, *javaClass, *constructor, converted.get());

        return object;
    }


    /**
     * Json Array
     */
    jni::jobject* operator()(const std::vector<mbgl::Value> &values) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/google/gson/JsonArray")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "()V");;
        static jni::jmethodID* add = &jni::GetMethodID(env, *javaClass, "add", "(Lcom/google/gson/JsonElement;)V");

        //Create json array
        jni::jobject* jarray = &jni::NewObject(env, *javaClass, *constructor);

        //Add values
        for (const auto &v : values) {
            jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, mbgl::Value::visit(v, *this));
            jni::CallMethod<void>(env, jarray, *add, converted.get());
        }

        return jarray;
    }

    /**
     * Json Object
     */
    jni::jobject* operator()(const std::unordered_map<std::string, mbgl::Value> &value) const {
        //TODO: clean up duplication here
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/google/gson/JsonObject")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "()V");;
        static jni::jmethodID* add = &jni::GetMethodID(env, *javaClass, "add", "(Ljava/lang/String;Lcom/google/gson/JsonElement;)V");

        //Create json object
        jni::jobject* jsonObject = &jni::NewObject(env, *javaClass, *constructor);

        //Add items
        for (auto &item : value) {
            jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, mbgl::Value::visit(item.second, *this));
            jni::LocalObject<jni::jobject> key = jni::NewLocalObject(env, *convert<jni::jobject*, std::string>(env, item.first));
            jni::CallMethod<void>(env, jsonObject, *add, key.get(), converted.get());
        }

        return jsonObject;
    }
};

template <>
struct Converter<jni::jobject*, std::unordered_map<std::string, mbgl::Value>> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const std::unordered_map<std::string, mbgl::Value>& value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/google/gson/JsonObject")).release();
        static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "()V");;
        static jni::jmethodID* add = &jni::GetMethodID(env, *javaClass, "add", "(Ljava/lang/String;Lcom/google/gson/JsonElement;)V");

        //Create json object
        jni::jobject* jsonObject = &jni::NewObject(env, *javaClass, *constructor);

        //Add items
        PropertyValueEvaluator evaluator {env};
        for (auto &item : value) {
            jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, mbgl::Value::visit(item.second, evaluator));
            jni::LocalObject<jni::jobject> key = jni::NewLocalObject(env, *convert<jni::jobject*, std::string>(env, item.first));
            jni::CallMethod<void>(env, jsonObject, *add, key.get(), converted.get());
        }

        return {jsonObject};
    }
};


template <>
struct Converter<jni::jobject*, mbgl::Feature> {
    Result<jni::jobject*> operator()(jni::JNIEnv& env, const mbgl::Feature& value) const {
        static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/services/commons/geojson/Feature")).release();
        static jni::jmethodID* fromGeometry = &jni::GetStaticMethodID(env, *javaClass, "fromGeometry", "(Lcom/mapbox/services/commons/geojson/Geometry;Lcom/google/gson/JsonObject;Ljava/lang/String;)Lcom/mapbox/services/commons/geojson/Feature;");

        //Convert Id
        FeatureIdVisitor idEvaluator;
        std::string id = (value.id) ? mapbox::geometry::identifier::visit(value.id.value(), idEvaluator) : "";
        jni::LocalObject<jni::jobject> jid = jni::NewLocalObject(env, *convert<jni::jobject*>(env, id));

        //Convert properties
        jni::LocalObject<jni::jobject> properties = jni::NewLocalObject(env, *convert<jni::jobject*>(env, value.properties));

        //Convert geometry
        jni::LocalObject<jni::jobject> geometry = jni::NewLocalObject(env, *convert<jni::jobject*>(env, value.geometry));

        //Create feature
        return {reinterpret_cast<jni::jobject*>(jni::CallStaticMethod<jni::jobject*>(env, *javaClass, *fromGeometry, geometry.get(), properties.get(), jid.get()))};
    }
};

template <>
struct Converter<jni::jarray<jni::jobject>*, std::vector<mbgl::Feature>> {
    Result<jni::jarray<jni::jobject>*> operator()(jni::JNIEnv& env, const std::vector<mbgl::Feature>& value) const {
        static jni::jclass* featureClass = jni::NewGlobalRef(env, &jni::FindClass(env, "com/mapbox/services/commons/geojson/Feature")).release();
        jni::jarray<jni::jobject>& jarray = jni::NewObjectArray(env, value.size(), *featureClass);

        for(size_t i = 0; i < value.size(); i = i + 1) {
            jni::LocalObject<jni::jobject> converted = jni::NewLocalObject(env, *convert<jni::jobject*, mbgl::Feature>(env, value.at(i)));
            jni::SetObjectArrayElement(env, jarray, i, converted.get());
        }

        return {&jarray};
    }
};

} // namespace conversion
} // namespace android
} // namespace mbgl
