#include "json_primitive.hpp"

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
        static auto javaClass = jni::Class<JsonPrimitive>::Singleton(env);
        static auto constructor = javaClass.GetConstructor<jni::String>(env);

        return javaClass.New(env, constructor,
            *jni::SeizeLocal(env, jni::Make<jni::String>(env, value)));
    }

    /**
     * Create a primitive containing a number value with type double
     */
    jni::Object<JsonPrimitive> operator()(const double value) const {
        static auto javaClass = jni::Class<JsonPrimitive>::Singleton(env);
        static auto constructor = javaClass.GetConstructor<jni::Number>(env);

        return javaClass.New(env, constructor,
            *jni::SeizeLocal(env, jni::Number(jni::Box(env, value))));
    }

    /**
     * Create a primitive containing a number value with type long
     */
    jni::Object<JsonPrimitive> operator()(const int64_t value) const {
        static auto javaClass = jni::Class<JsonPrimitive>::Singleton(env);
        static auto constructor = javaClass.GetConstructor<jni::Number>(env);

        return javaClass.New(env, constructor,
            *jni::SeizeLocal(env, jni::Number(jni::Box(env, value))));
    }

    /**
     * Create a primitive containing a number value with type long
     */
    jni::Object<JsonPrimitive> operator()(const uint64_t value) const {
        static auto javaClass = jni::Class<JsonPrimitive>::Singleton(env);
        static auto constructor = javaClass.GetConstructor<jni::Number>(env);

        return javaClass.New(env, constructor,
            *jni::SeizeLocal(env, jni::Number(jni::Box(env, int64_t(value))))); // TODO: should use BigInteger
    }

    /**
     * Create a primitive containing a boolean value
     */
    jni::Object<JsonPrimitive> operator()(const bool value) const {
        static auto javaClass = jni::Class<JsonPrimitive>::Singleton(env);
        static auto constructor = javaClass.GetConstructor<jni::Boolean>(env);

        return javaClass.New(env, constructor,
            *jni::SeizeLocal(env, jni::Box(env, value ? jni::jni_true : jni::jni_false)));
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
    static auto javaClass = jni::Class<JsonPrimitive>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::jboolean ()>(env, "isBoolean");
    return jsonPrimitive.Call(env, method);
}

bool JsonPrimitive::isString(JNIEnv& env, jni::Object<JsonPrimitive> jsonPrimitive) {
    static auto javaClass = jni::Class<JsonPrimitive>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::jboolean ()>(env, "isString");
    return jsonPrimitive.Call(env, method);
}

bool JsonPrimitive::isNumber(JNIEnv& env, jni::Object<JsonPrimitive> jsonPrimitive) {
    static auto javaClass = jni::Class<JsonPrimitive>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::jboolean ()>(env, "isNumber");
    return jsonPrimitive.Call(env, method);
}

bool JsonPrimitive::getAsBoolean(JNIEnv& env, jni::Object<JsonPrimitive> jsonPrimitive) {
    static auto javaClass = jni::Class<JsonPrimitive>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::jboolean ()>(env, "getAsBoolean");
    return jsonPrimitive.Call(env, method);
}

std::string JsonPrimitive::getAsString(JNIEnv& env, jni::Object<JsonPrimitive> jsonPrimitive) {
    static auto javaClass = jni::Class<JsonPrimitive>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::String ()>(env, "getAsString");
    return jni::Make<std::string>(env, *jni::SeizeLocal(env, jsonPrimitive.Call(env, method)));
}

double JsonPrimitive::getAsDouble(JNIEnv& env, jni::Object<JsonPrimitive> jsonPrimitive) {
    static auto javaClass = jni::Class<JsonPrimitive>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::jdouble ()>(env, "getAsDouble");
    return jsonPrimitive.Call(env, method);
}

void JsonPrimitive::registerNative(jni::JNIEnv &env) {
    jni::Class<JsonPrimitive>::Singleton(env);
}

} // namespace gson
} // namespace android
} // namespace mbgl