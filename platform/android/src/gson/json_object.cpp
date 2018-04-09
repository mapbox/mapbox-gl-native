#include "json_object.hpp"

#include "json_element.hpp"

#include "../java/util.hpp"

namespace mbgl {
namespace android {
namespace gson {


jni::Object<JsonObject> JsonObject::New(jni::JNIEnv& env, const std::unordered_map<std::string, mapbox::geometry::value>& values) {
    static auto constructor = JsonObject::javaClass.GetConstructor(env);
    static auto addMethod = JsonObject::javaClass.GetMethod<void (jni::String, jni::Object<JsonElement>)>(env, "add");

    jni::Object<JsonObject> jsonObject = JsonObject::javaClass.New(env, constructor);

    for (auto &item : values) {
        jni::Object<JsonElement> jsonElement = JsonElement::New(env, item.second);
        jni::String key = jni::Make<jni::String>(env, item.first);
        jsonObject.Call(env, addMethod, key, jsonElement);
        jni::DeleteLocalRef(env, jsonElement);
        jni::DeleteLocalRef(env, key);
    }

    return jsonObject;
}

template <typename F> // void (jni::String, jni::Object<gson::JsonElement>)
static void iterateEntrySet(jni::JNIEnv& env, jni::Object<JsonObject> jsonObject, F callback) {
    // Get Set<Map.Entry<String, JsonElement>>
    static auto method = JsonObject::javaClass.GetMethod<jni::Object<java::util::Set> ()>(env, "entrySet");
    auto entrySet = jsonObject.Call(env, method);
    jni::Array<jni::Object<java::util::Map::Entry>> entryArray = java::util::Set::toArray<java::util::Map::Entry>(env, entrySet);

    size_t size = entryArray.Length(env);
    for (size_t i = 0; i < size; i++) {
        auto entry = entryArray.Get(env, i);
        if (entry) {
            // Convert
            auto jKey = java::util::Map::Entry::getKey<jni::ObjectTag>(env, entry);
            auto jKeyString = jni::String(reinterpret_cast<jni::jstring*>(jKey.Get()));
            auto jValue = java::util::Map::Entry::getValue<gson::JsonElement>(env, entry);

            // Callback
            callback(jKeyString, jValue);

            // Cleanup
            // Skip jKey as it points to the same as jKeyString
            jni::DeleteLocalRef(env, jKeyString);
            jni::DeleteLocalRef(env, jValue);
        }
        jni::DeleteLocalRef(env, entry);
    }

    jni::DeleteLocalRef(env, entrySet);
    jni::DeleteLocalRef(env, entryArray);
}

mapbox::geometry::property_map JsonObject::convert(jni::JNIEnv &env, jni::Object<JsonObject> jsonObject) {
    mapbox::geometry::property_map map;

    if (jsonObject) {
        iterateEntrySet(env, jsonObject, [&map, &env](jni::String jId, jni::Object<gson::JsonElement> jsonElement) {
            map[jni::Make<std::string>(env, jId)] = JsonElement::convert(env, jsonElement);
        });
    }

    return map;
}

void JsonObject::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    javaClass = *jni::Class<JsonObject>::Find(env).NewGlobalRef(env).release();
}

jni::Class<JsonObject> JsonObject::javaClass;

} // namespace gson
} // namespace android
} // namespace mbgl