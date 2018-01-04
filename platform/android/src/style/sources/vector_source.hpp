#pragma once

#include "source.hpp"
#include <mbgl/style/sources/vector_source.hpp>
#include "../../geojson/feature.hpp"
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class VectorSource : public Source {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/VectorSource"; };

    static jni::Class<VectorSource> javaClass;

    static void registerNative(jni::JNIEnv&);

    VectorSource(jni::JNIEnv&, jni::String, jni::Object<>);

    VectorSource(jni::JNIEnv&, mbgl::style::Source&, AndroidRendererFrontend&);

    ~VectorSource();

    jni::Array<jni::Object<geojson::Feature>> querySourceFeatures(jni::JNIEnv&, jni::Array<jni::String>,
                                                                  jni::Array<jni::Object<>> jfilter);

    jni::String getURL(jni::JNIEnv&);

private:
    jni::Object<Source> createJavaPeer(jni::JNIEnv&);

}; // class VectorSource

} // namespace android
} // namespace mbgl
