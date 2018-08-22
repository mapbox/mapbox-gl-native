#pragma once

#include "json_element.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace gson {

class JsonObject {
public:
    using SuperTag = JsonElement;
    static constexpr auto Name() { return "com/google/gson/JsonObject"; };

    static jni::Local<jni::Object<JsonObject>> New(jni::JNIEnv&, const mbgl::PropertyMap&);
    static mbgl::PropertyMap convert(JNIEnv&, const jni::Object<JsonObject>&);

    static void registerNative(jni::JNIEnv&);
};

} // namespace gson
} // namespace android
} // namespace mbgl