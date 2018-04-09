#include "json_primitive.hpp"
#include "../java/lang.hpp"

namespace mbgl {
namespace android {
namespace gson {

/**
 * Turn mapbox::geometry::value into Java Gson JsonPrimitives
 */
class JsonPrimitiveEvaluator {
public:

    jni::JNIEnv& env;

    /**
     * Create a null primitive
     */
    jni::Object<JsonPrimitive> operator()(const mapbox::geometry::null_value_t) const {
        return jni::Object<JsonPrimitive>();
    }

    /**
     * Create a primitive containing a string value
     */
    jni::Object<JsonPrimitive> operator()(const std::string value) const {
        static auto constructor = JsonPrimitive::javaClass.GetConstructor<jni::String>(env);
        auto jvalue = jni::Make<jni::String>(env, value);
        auto jsonPrimitive = JsonPrimitive::javaClass.New(env, constructor, jvalue);
        jni::DeleteLocalRef(env, jvalue);
        return jsonPrimitive;
    }

    /**
     * Create a primitive containing a number value with type double
     */
    jni::Object<JsonPrimitive> operator()(const double value) const {
        static auto constructor = JsonPrimitive::javaClass.GetConstructor<jni::Object<java::lang::Number>>(env);
        auto boxedValue = java::lang::Double::valueOf(env, value);
        auto number = jni::Cast(env, boxedValue, java::lang::Number::javaClass);
        auto jsonPrimitive = JsonPrimitive::javaClass.New(env, constructor, number);
        jni::DeleteLocalRef(env, boxedValue);
        return jsonPrimitive;
    }

    /**
     * Create a primitive containing a number value with type long
     */
    jni::Object<JsonPrimitive> operator()(const int64_t value) const {
        static auto constructor = JsonPrimitive::javaClass.GetConstructor<jni::Object<java::lang::Number>>(env);
        auto boxedValue = java::lang::Long::valueOf(env, value);
        auto number = jni::Cast(env, boxedValue, java::lang::Number::javaClass);
        auto jsonPrimitive =  JsonPrimitive::javaClass.New(env, constructor, number);
        jni::DeleteLocalRef(env, boxedValue);
        return jsonPrimitive;
    }

    /**
     * Create a primitive containing a number value with type long
     */
    jni::Object<JsonPrimitive> operator()(const uint64_t value) const {
        static auto constructor = JsonPrimitive::javaClass.GetConstructor<jni::Object<java::lang::Number>>(env);
        auto boxedValue = java::lang::Long::valueOf(env, value);
        auto number = jni::Cast(env, boxedValue, java::lang::Number::javaClass);
        auto jsonPrimitive = JsonPrimitive::javaClass.New(env, constructor, number);
        jni::DeleteLocalRef(env, boxedValue);
        return jsonPrimitive;
    }

    /**
     * Create a primitive containing a boolean value
     */
    jni::Object<JsonPrimitive> operator()(const bool value) const {
        static auto constructor = JsonPrimitive::javaClass.GetConstructor<jni::Object<java::lang::Boolean>>(env);
        auto boxedValue = java::lang::Boolean::valueOf(env, value);
        auto jsonPrimitive = JsonPrimitive::javaClass.New(env, constructor, boxedValue);
        jni::DeleteLocalRef(env, boxedValue);
        return jsonPrimitive;
    }
};

jni::Object<JsonPrimitive> JsonPrimitive::New(jni::JNIEnv &env, const value& value) {
    JsonPrimitiveEvaluator evaluator { env };
    return value::visit(value, evaluator);
}

JsonPrimitive::value JsonPrimitive::convert(jni::JNIEnv &env, jni::Object<JsonPrimitive> jsonPrimitive) {
    value value;
    if (jsonPrimitive) {
        if (isBoolean(env, jsonPrimitive)) {
            value = getAsBoolean(env, jsonPrimitive);
        } else if (isNumber(env, jsonPrimitive)) {
            //TODO: how to differentiate types here?
            value = getAsDouble(env, jsonPrimitive);
        } else if (isString(env, jsonPrimitive)) {
            value = getAsString(env, jsonPrimitive);
        } else {
            value = mapbox::geometry::null_value;
        }
    }
    return value;
}

bool JsonPrimitive::isBoolean(JNIEnv& env, jni::Object<JsonPrimitive> jsonPrimitive) {
    static auto method = JsonPrimitive::javaClass.GetMethod<jni::jboolean ()>(env, "isBoolean");
    return jsonPrimitive.Call(env, method);
}

bool JsonPrimitive::isString(JNIEnv& env, jni::Object<JsonPrimitive> jsonPrimitive) {
    static auto method = JsonPrimitive::javaClass.GetMethod<jni::jboolean ()>(env, "isString");
    return jsonPrimitive.Call(env, method);
}

bool JsonPrimitive::isNumber(JNIEnv& env, jni::Object<JsonPrimitive> jsonPrimitive) {
    static auto method = JsonPrimitive::javaClass.GetMethod<jni::jboolean ()>(env, "isNumber");
    return jsonPrimitive.Call(env, method);
}

bool JsonPrimitive::getAsBoolean(JNIEnv& env, jni::Object<JsonPrimitive> jsonPrimitive) {
    static auto method = JsonPrimitive::javaClass.GetMethod<jni::jboolean ()>(env, "getAsBoolean");
    return jsonPrimitive.Call(env, method);
}

std::string JsonPrimitive::getAsString(JNIEnv& env, jni::Object<JsonPrimitive> jsonPrimitive) {
    static auto method = JsonPrimitive::javaClass.GetMethod<jni::String ()>(env, "getAsString");
    auto jString = jsonPrimitive.Call(env, method);
    auto string = jni::Make<std::string>(env, jString);
    jni::DeleteLocalRef(env, jString);
    return string;
}

double JsonPrimitive::getAsDouble(JNIEnv& env, jni::Object<JsonPrimitive> jsonPrimitive) {
    static auto method = JsonPrimitive::javaClass.GetMethod<jni::jdouble ()>(env, "getAsDouble");
    return jsonPrimitive.Call(env, method);
}

void JsonPrimitive::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<JsonPrimitive>::Find(env).NewGlobalRef(env).release();
}

jni::Class<JsonPrimitive> JsonPrimitive::javaClass;

} // namespace gson
} // namespace android
} // namespace mbgl