#include "json_element.hpp"
#include "json_array.hpp"
#include "json_object.hpp"
#include "json_primitive.hpp"

namespace mbgl {
namespace android {
namespace gson {

jni::Local<jni::Object<JsonElement>> JsonElement::New(jni::JNIEnv& env, const mbgl::Value& value) {
    static auto& primitive = jni::Class<JsonPrimitive>::Singleton(env);
    static auto stringConstructor = primitive.GetConstructor<jni::String>(env);
    static auto numberConstructor = primitive.GetConstructor<jni::Number>(env);
    static auto booleanConstructor = primitive.GetConstructor<jni::Boolean>(env);

    return value.match(
        [&] (const mbgl::NullValue&) {
            return jni::Local<jni::Object<JsonElement>>();
        },
        [&] (const std::string& value) {
            return primitive.New(env, stringConstructor, jni::Make<jni::String>(env, value));
        },
        [&] (const double value) {
            return primitive.New(env, numberConstructor, jni::Box(env, value));
        },
        [&] (const int64_t value) {
            return primitive.New(env, numberConstructor, jni::Box(env, value));
        },
        [&] (const uint64_t value) {
            return primitive.New(env, numberConstructor, jni::Box(env, int64_t(value))); // TODO: should use BigInteger
        },
        [&] (const bool value) {
            return primitive.New(env, booleanConstructor, jni::Box(env, value ? jni::jni_true : jni::jni_false));
        },
        [&] (const std::vector<mbgl::Value>& values) {
            return JsonArray::New(env, values);
        },
        [&] (const mbgl::PropertyMap& values) {
            return JsonObject::New(env, values);
        }
    );
}

mbgl::Value JsonElement::convert(jni::JNIEnv &env, const jni::Object<JsonElement>& jsonElement) {
    if (!jsonElement) {
        return mbgl::NullValue();
    }

    static auto& elementClass = jni::Class<JsonElement>::Singleton(env);
    static auto isJsonObject = elementClass.GetMethod<jni::jboolean ()>(env, "isJsonObject");
    static auto isJsonArray = elementClass.GetMethod<jni::jboolean ()>(env, "isJsonArray");
    static auto isJsonPrimitive = elementClass.GetMethod<jni::jboolean ()>(env, "isJsonPrimitive");

    static auto& primitiveClass = jni::Class<JsonPrimitive>::Singleton(env);
    static auto isBoolean = primitiveClass.GetMethod<jni::jboolean ()>(env, "isBoolean");
    static auto isString = primitiveClass.GetMethod<jni::jboolean ()>(env, "isString");
    static auto isNumber = primitiveClass.GetMethod<jni::jboolean ()>(env, "isNumber");
    static auto getAsBoolean = primitiveClass.GetMethod<jni::jboolean ()>(env, "getAsBoolean");
    static auto getAsString = primitiveClass.GetMethod<jni::String ()>(env, "getAsString");
    static auto getAsDouble = primitiveClass.GetMethod<jni::jdouble ()>(env, "getAsDouble");

    if (jsonElement.Call(env, isJsonPrimitive)) {
        auto primitive = jni::Cast(env, primitiveClass, jsonElement);
        if (primitive.Call(env, isBoolean)) {
            return bool(primitive.Call(env, getAsBoolean));
        } else if (primitive.Call(env, isNumber)) {
            return primitive.Call(env, getAsDouble);
        } else if (primitive.Call(env, isString)) {
            return jni::Make<std::string>(env, primitive.Call(env, getAsString));
        } else {
            return mbgl::NullValue();
        }
    } else if (jsonElement.Call(env, isJsonObject)) {
        return JsonObject::convert(env, jni::Cast(env, jni::Class<JsonObject>::Singleton(env), jsonElement));
    } else if (jsonElement.Call(env, isJsonArray)) {
        return JsonArray::convert(env, jni::Cast(env, jni::Class<JsonArray>::Singleton(env), jsonElement));
    } else {
        return mbgl::NullValue();
    }
}

void JsonElement::registerNative(jni::JNIEnv &env) {
    jni::Class<JsonElement>::Singleton(env);
}

} // namespace gson
} // namespace android
} // namespace mbgl