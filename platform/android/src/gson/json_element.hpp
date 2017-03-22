#pragma once

#include <mapbox/geometry.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace gson {

class JsonElement : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/google/gson/JsonElement"; };

    static mapbox::geometry::value convert(JNIEnv&, jni::Object<JsonElement>);

    static bool isJsonObject(JNIEnv&, jni::Object<JsonElement>);

    static bool isJsonArray(JNIEnv&, jni::Object<JsonElement>);

    static bool isJsonPrimitive(JNIEnv&, jni::Object<JsonElement>);

    static bool isJsonNull(JNIEnv&, jni::Object<JsonElement>);

    static jni::Class<JsonElement> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace gson
} // namespace android
} // namespace mbgl