#pragma once

#include "source.hpp"
#include <mbgl/style/sources/vector_source.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class VectorSource : public Source {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/VectorSource"; };

    static jni::Class<VectorSource> javaClass;

    static void registerNative(jni::JNIEnv&);

    VectorSource(jni::JNIEnv&, jni::String, jni::Object<>);

    VectorSource(mbgl::Map&, mbgl::style::VectorSource&);

    ~VectorSource();

    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class VectorSource

} // namespace android
} // namespace mbgl
