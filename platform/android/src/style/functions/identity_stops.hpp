#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class IdentityStops : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/functions/stops/IdentityStops"; };

    static jni::Object<IdentityStops> New(jni::JNIEnv&);

    static jni::Class<IdentityStops> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
