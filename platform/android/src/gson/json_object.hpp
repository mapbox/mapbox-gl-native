#pragma once

#include <mapbox/geometry.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace gson {

class JsonObject : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/google/gson/JsonObject"; };

    static jni::Object<JsonObject> New(jni::JNIEnv&, const std::unordered_map<std::string, mapbox::geometry::value>&);

    static mapbox::geometry::property_map convert(JNIEnv&, jni::Object<JsonObject>);

    static jni::Class<JsonObject> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace gson
} // namespace android
} // namespace mbgl