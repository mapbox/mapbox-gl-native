#pragma once

#include <mapbox/geometry.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace gson {

class JsonArray : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/google/gson/JsonArray"; };

    static std::vector<mapbox::geometry::value> convert(JNIEnv&, jni::Object<JsonArray>);

    static jni::Class<JsonArray> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace gson
} // namespace android
} // namespace mbgl