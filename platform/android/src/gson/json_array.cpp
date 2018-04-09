#include "json_array.hpp"

#include "json_element.hpp"

namespace mbgl {
namespace android {
namespace gson {

jni::Object<JsonArray> JsonArray::New(jni::JNIEnv& env, const std::vector<mapbox::geometry::value>& values){
    static auto constructor = JsonArray::javaClass.GetConstructor(env);
    static auto addMethod = JsonArray::javaClass.GetMethod<void (jni::Object<JsonElement>)>(env, "add");

    auto jsonArray = JsonArray::javaClass.New(env, constructor);

    for (const auto &v : values) {
        auto jsonElement = JsonElement::New(env, v);
        jsonArray.Call(env, addMethod, jsonElement);
        jni::DeleteLocalRef(env, jsonElement);
    }

    return jsonArray;
}

std::vector<mapbox::geometry::value> JsonArray::convert(jni::JNIEnv& env, const jni::Object<JsonArray> jsonArray) {
    std::vector<mapbox::geometry::value> values;

    if (jsonArray) {
        static auto getMethod = JsonArray::javaClass.GetMethod<jni::Object<JsonElement> (jni::jint)>(env, "get");
        static auto sizeMethod = JsonArray::javaClass.GetMethod<jni::jint ()>(env, "size");

        int size = jsonArray.Call(env, sizeMethod);
        values.reserve(uint(size));

        for (int i = 0; i < size; i++) {
            auto entry = jsonArray.Call(env, getMethod, i);
            if (entry) {
                values.push_back(JsonElement::convert(env, entry));
            }
            jni::DeleteLocalRef(env, entry);
        }
    }

    return values;
}

void JsonArray::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    javaClass = *jni::Class<JsonArray>::Find(env).NewGlobalRef(env).release();
}

jni::Class<JsonArray> JsonArray::javaClass;

} // namespace gson
} // namespace android
} // namespace mbgl