#pragma once

#include "json_element.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace gson {

class JsonPrimitive {
public:
    using SuperTag = JsonElement;
    static constexpr auto Name() { return "com/google/gson/JsonPrimitive"; };

    static void registerNative(jni::JNIEnv&);
};

} // namespace gson
} // namespace android
} // namespace mbgl