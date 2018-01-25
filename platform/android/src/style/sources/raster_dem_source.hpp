#pragma once

#include "source.hpp"
#include <mbgl/style/sources/raster_dem_source.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class RasterDEMSource : public Source {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/RasterDemSource"; };

    static jni::Class<RasterDEMSource> javaClass;

    static void registerNative(jni::JNIEnv&);

    RasterDEMSource(jni::JNIEnv&, jni::String, jni::Object<>, jni::jint);

    RasterDEMSource(jni::JNIEnv&, mbgl::style::Source&, AndroidRendererFrontend&);

    ~RasterDEMSource();

    jni::String getURL(jni::JNIEnv&);

private:
    jni::Object<Source> createJavaPeer(jni::JNIEnv&);

}; // class RasterDEMSource

} // namespace android
} // namespace mbgl
