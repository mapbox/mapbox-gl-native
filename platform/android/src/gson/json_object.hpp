#pragma once

#include "json_element.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace gson {

class JsonObject : public JsonElement {
public:
    static constexpr auto Name() { return "com/google/gson/JsonObject"; };

    static jni::Object<JsonObject> New(jni::JNIEnv&, const mbgl::PropertyMap&);
    static mbgl::PropertyMap convert(JNIEnv&, jni::Object<JsonObject>);

    static void registerNative(jni::JNIEnv&);
};

} // namespace gson
} // namespace android
} // namespace mbgl