#pragma once

#include <mapbox/geometry.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace gson {

class JsonPrimitive : private mbgl::util::noncopyable {
public:
    using value = mapbox::util::variant<mapbox::geometry::null_value_t, bool, uint64_t, int64_t, double, std::string>;

    static constexpr auto Name() { return "com/google/gson/JsonPrimitive"; };

    static value convert(JNIEnv&, jni::Object<JsonPrimitive>);

    static bool isBoolean(JNIEnv&, jni::Object<JsonPrimitive>);

    static bool isString(JNIEnv&, jni::Object<JsonPrimitive>);

    static bool isNumber(JNIEnv&, jni::Object<JsonPrimitive>);

    static bool getAsBoolean(JNIEnv&, jni::Object<JsonPrimitive>);

    static std::string getAsString(JNIEnv&, jni::Object<JsonPrimitive>);

    static double getAsDouble(JNIEnv&, jni::Object<JsonPrimitive>);

    static jni::Class<JsonPrimitive> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace gson
} // namespace android
} // namespace mbgl