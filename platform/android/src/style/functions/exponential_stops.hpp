#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <jni/jni.hpp>

#include "../../java/lang.hpp"
#include "stop.hpp"

namespace mbgl {
namespace android {

class ExponentialStops : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/functions/stops/ExponentialStops"; };

    static jni::Object<ExponentialStops> New(jni::JNIEnv&, jni::Object<java::lang::Float>, jni::Array<jni::Object<Stop>>);

    static jni::Class<ExponentialStops> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
