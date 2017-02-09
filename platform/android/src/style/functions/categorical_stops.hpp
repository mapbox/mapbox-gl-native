#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <jni/jni.hpp>

#include "stop.hpp"

namespace mbgl {
namespace android {

class CategoricalStops : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/functions/stops/CategoricalStops"; };

    static jni::Object<CategoricalStops> New(jni::JNIEnv&, jni::Array<jni::Object<Stop>>);

    static jni::Class<CategoricalStops> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
