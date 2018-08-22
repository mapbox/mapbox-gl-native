#pragma once

#include "source.hpp"
#include <mbgl/style/sources/vector_source.hpp>
#include "../../geojson/feature.hpp"
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class VectorSource : public Source {
public:
    using SuperTag = Source;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/VectorSource"; };

    static void registerNative(jni::JNIEnv&);

    VectorSource(jni::JNIEnv&, const jni::String&, const jni::Object<>&);
    VectorSource(jni::JNIEnv&, mbgl::style::Source&, AndroidRendererFrontend&);
    ~VectorSource();

private:
    jni::Local<jni::Array<jni::Object<geojson::Feature>>> querySourceFeatures(jni::JNIEnv&, const jni::Array<jni::String>&,
                                                                  const jni::Array<jni::Object<>>& jfilter);

    jni::Local<jni::String> getURL(jni::JNIEnv&);

    jni::Local<jni::Object<Source>> createJavaPeer(jni::JNIEnv&);

}; // class VectorSource

} // namespace android
} // namespace mbgl
