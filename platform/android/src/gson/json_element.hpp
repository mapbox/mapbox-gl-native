#pragma once

#include <mbgl/util/feature.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace gson {

class JsonElement : public jni::ObjectTag {
public:
    static constexpr auto Name() { return "com/google/gson/JsonElement"; };

    static jni::Object<JsonElement> New(jni::JNIEnv&, const mbgl::Value&);
    static mbgl::Value convert(JNIEnv&, jni::Object<JsonElement>);

    static void registerNative(jni::JNIEnv&);
};

} // namespace gson
} // namespace android
} // namespace mbgl