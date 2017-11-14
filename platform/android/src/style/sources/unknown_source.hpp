#pragma once

#include "source.hpp"
#include <mbgl/style/source.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class UnknownSource : public Source {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/UnknownSource"; };

    static jni::Class<UnknownSource> javaClass;

    static void registerNative(jni::JNIEnv&);

    UnknownSource(jni::JNIEnv&, mbgl::style::Source&, AndroidRendererFrontend&);

    ~UnknownSource() = default;

private:
    jni::Object<Source> createJavaPeer(jni::JNIEnv&);

}; // class UnknownSource

} // namespace android
} // namespace mbgl
