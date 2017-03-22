#include "json_primitive.hpp"

namespace mbgl {
namespace android {
namespace gson {

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