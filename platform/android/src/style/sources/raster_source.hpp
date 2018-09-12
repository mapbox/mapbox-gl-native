#pragma once

#include "source.hpp"
#include <mbgl/style/sources/raster_source.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class RasterSource : public Source {
public:
    using SuperTag = Source;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/RasterSource"; };

    static void registerNative(jni::JNIEnv&);

    RasterSource(jni::JNIEnv&, const jni::String&, const jni::Object<>&, jni::jint);
    RasterSource(jni::JNIEnv&, mbgl::style::Source&, AndroidRendererFrontend&);
    ~RasterSource();

    jni::Local<jni::String> getURL(jni::JNIEnv&);

private:
    jni::Local<jni::Object<Source>> createJavaPeer(jni::JNIEnv&);

}; // class RasterSource

} // namespace android
} // namespace mbgl
