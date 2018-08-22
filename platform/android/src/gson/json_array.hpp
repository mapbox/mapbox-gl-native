#pragma once

#include "json_element.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace gson {

class JsonArray : public JsonElement {
public:
    static constexpr auto Name() { return "com/google/gson/JsonArray"; };

    static jni::Object<JsonArray> New(jni::JNIEnv&, const std::vector<mbgl::Value>&);
    static std::vector<mbgl::Value> convert(JNIEnv&, jni::Object<JsonArray>);

    static void registerNative(jni::JNIEnv&);
};

} // namespace gson
} // namespace android
} // namespace mbgl