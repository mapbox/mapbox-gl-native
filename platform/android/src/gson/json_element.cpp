#include "json_element.hpp"

#include "json_array.hpp"
#include "json_object.hpp"
#include "json_primitive.hpp"

#include <mapbox/geometry/feature.hpp>
#include <mapbox/variant.hpp>

namespace mbgl {
namespace android {
namespace gson {

mapbox::geometry::value JsonElement::convert(jni::JNIEnv &env, jni::Object<JsonElement> jsonElement) {
    mapbox::geometry::value value;

    if (jsonElement) {
        if (isJsonPrimitive(env, jsonElement)) {
            auto primitive = JsonPrimitive::convert(env, jni::Cast(env, jsonElement, JsonPrimitive::javaClass));
            value = mapbox::util::apply_visitor([](auto t) { return mapbox::geometry::value { t }; }, primitive);
        } else if (isJsonObject(env, jsonElement)) {
            mapbox::geometry::property_map map = JsonObject::convert(env, jni::Cast(env, jsonElement, JsonObject::javaClass));
            value = mapbox::util::recursive_wrapper<std::unordered_map<std::string, mapbox::geometry::value>> { map } ;
        } else  if (isJsonArray(env, jsonElement)) {
            value = JsonArray::convert(env, jni::Cast(env, jsonElement, JsonArray::javaClass));
        } else {
            value = mapbox::geometry::null_value;
        }
    }
    return value;
}

bool JsonElement::isJsonObject(JNIEnv& env, jni::Object<JsonElement> jsonElement) {
    static auto method = JsonElement::javaClass.GetMethod<jni::jboolean ()>(env, "isJsonObject");
    return jsonElement.Call(env, method);
}

bool JsonElement::isJsonArray(JNIEnv& env, jni::Object<JsonElement> jsonElement) {
    static auto method = JsonElement::javaClass.GetMethod<jni::jboolean ()>(env, "isJsonArray");
    return jsonElement.Call(env, method);
}

bool JsonElement::isJsonPrimitive(JNIEnv& env, jni::Object<JsonElement> jsonElement) {
    static auto method = JsonElement::javaClass.GetMethod<jni::jboolean ()>(env, "isJsonPrimitive");
    return jsonElement.Call(env, method);
}

bool JsonElement::isJsonNull(JNIEnv& env, jni::Object<JsonElement> jsonElement) {
    static auto method = JsonElement::javaClass.GetMethod<jni::jboolean ()>(env, "isJsonNull");
    return jsonElement.Call(env, method);
}

void JsonElement::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<JsonElement>::Find(env).NewGlobalRef(env).release();
}

jni::Class<JsonElement> JsonElement::javaClass;

} // namespace gson
} // namespace android
} // namespace mbgl