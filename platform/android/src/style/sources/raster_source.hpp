#pragma once

#include "source.hpp"
#include <mbgl/style/sources/raster_source.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class RasterSource : public Source {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/RasterSource"; };

    static jni::Class<RasterSource> javaClass;

    static void registerNative(jni::JNIEnv&);

    RasterSource(jni::JNIEnv&, jni::String, jni::Object<>, jni::jint);

    RasterSource(mbgl::Map&, mbgl::style::RasterSource&);

    ~RasterSource();

    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class RasterSource

} // namespace android
} // namespace mbgl
