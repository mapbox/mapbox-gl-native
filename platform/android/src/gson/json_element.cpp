#include "json_element.hpp"

#include "json_array.hpp"
#include "json_object.hpp"
#include "json_primitive.hpp"

#include <mapbox/geometry/feature.hpp>
#include <mapbox/variant.hpp>

namespace mbgl {
namespace android {
namespace gson {

/**
 * Turn mapbox::geometry::value into Java Gson JsonElement
 */
class JsonElementEvaluator {
public:

    jni::JNIEnv& env;

    jni::Object<JsonElement> operator()(const JsonPrimitive::value value) const {
        return JsonPrimitive::New(env, value);
    }

    jni::Object<JsonElement> operator()(const std::vector<mapbox::geometry::value> &values) const {
        return JsonArray::New(env, values);
    }

    jni::Object<JsonElement> operator()(const std::unordered_map<std::string, mapbox::geometry::value> &values) const {
        return JsonObject::New(env, values);
    }
};


jni::Object<JsonElement> JsonElement::New(jni::JNIEnv& env, const mapbox::geometry::value& value) {
  JsonElementEvaluator evaluator { env } ;
  return mapbox::geometry::value::visit(value, evaluator);
}

mapbox::geometry::value JsonElement::convert(jni::JNIEnv &env, jni::Object<JsonElement> jsonElement) {
    mapbox::geometry::value value;

    if (jsonElement) {
        if (isJsonPrimitive(env, jsonElement)) {
            static auto primitiveClass = jni::Class<JsonPrimitive>::Singleton(env);
            auto primitive = JsonPrimitive::convert(env, jni::Cast(env, primitiveClass, jsonElement));
            value = mapbox::util::apply_visitor([](auto t) { return mapbox::geometry::value { t }; }, primitive);
        } else if (isJsonObject(env, jsonElement)) {
            static auto objectClass = jni::Class<JsonObject>::Singleton(env);
            mapbox::geometry::property_map map = JsonObject::convert(env, jni::Cast(env, objectClass, jsonElement));
            value = mapbox::util::recursive_wrapper<std::unordered_map<std::string, mapbox::geometry::value>> { map } ;
        } else  if (isJsonArray(env, jsonElement)) {
            static auto arrayClass = jni::Class<JsonArray>::Singleton(env);
            value = JsonArray::convert(env, jni::Cast(env, arrayClass, jsonElement));
        } else {
            value = mapbox::geometry::null_value;
        }
    }
    return value;
}

bool JsonElement::isJsonObject(JNIEnv& env, jni::Object<JsonElement> jsonElement) {
    static auto javaClass = jni::Class<JsonElement>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::jboolean ()>(env, "isJsonObject");
    return jsonElement.Call(env, method);
}

bool JsonElement::isJsonArray(JNIEnv& env, jni::Object<JsonElement> jsonElement) {
    static auto javaClass = jni::Class<JsonElement>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::jboolean ()>(env, "isJsonArray");
    return jsonElement.Call(env, method);
}

bool JsonElement::isJsonPrimitive(JNIEnv& env, jni::Object<JsonElement> jsonElement) {
    static auto javaClass = jni::Class<JsonElement>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::jboolean ()>(env, "isJsonPrimitive");
    return jsonElement.Call(env, method);
}

bool JsonElement::isJsonNull(JNIEnv& env, jni::Object<JsonElement> jsonElement) {
    static auto javaClass = jni::Class<JsonElement>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::jboolean ()>(env, "isJsonNull");
    return jsonElement.Call(env, method);
}

void JsonElement::registerNative(jni::JNIEnv &env) {
    jni::Class<JsonElement>::Singleton(env);
}

} // namespace gson
} // namespace android
} // namespace mbgl