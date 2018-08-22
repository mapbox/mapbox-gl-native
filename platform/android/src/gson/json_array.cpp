#include "json_array.hpp"
#include "json_element.hpp"

namespace mbgl {
namespace android {
namespace gson {

jni::Local<jni::Object<JsonArray>> JsonArray::New(jni::JNIEnv& env, const std::vector<mbgl::Value>& values){
    static auto& javaClass = jni::Class<JsonArray>::Singleton(env);
    static auto constructor = javaClass.GetConstructor(env);
    static auto addMethod = javaClass.GetMethod<void (jni::Object<JsonElement>)>(env, "add");

    auto jsonArray = javaClass.New(env, constructor);

    for (const auto &v : values) {
        jsonArray.Call(env, addMethod, JsonElement::New(env, v));
    }

    return jsonArray;
}

std::vector<mbgl::Value> JsonArray::convert(jni::JNIEnv& env, const jni::Object<JsonArray>& jsonArray) {
    std::vector<mbgl::Value> values;

    if (jsonArray) {
        static auto& javaClass = jni::Class<JsonArray>::Singleton(env);
        static auto getMethod = javaClass.GetMethod<jni::Object<JsonElement> (jni::jint)>(env, "get");
        static auto sizeMethod = javaClass.GetMethod<jni::jint ()>(env, "size");

        int size = jsonArray.Call(env, sizeMethod);
        values.reserve(uint(size));

        for (int i = 0; i < size; i++) {
            auto entry = jsonArray.Call(env, getMethod, i);
            if (entry) {
                values.push_back(JsonElement::convert(env, entry));
            }
        }
    }

    return values;
}

void JsonArray::registerNative(jni::JNIEnv& env) {
    jni::Class<JsonArray>::Singleton(env);
}

} // namespace gson
} // namespace android
} // namespace mbgl