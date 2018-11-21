#pragma once

#include "source.hpp"
#include "../../geometry/lat_lng_quad.hpp"
#include <mbgl/style/sources/image_source.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class Bitmap;

class ImageSource : public Source {
public:
    using SuperTag = Source;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/ImageSource"; };

    static void registerNative(jni::JNIEnv&);

    ImageSource(jni::JNIEnv&, const jni::String&, const jni::Object<LatLngQuad>&);

    ImageSource(jni::JNIEnv&, mbgl::style::Source&, AndroidRendererFrontend&);

    ~ImageSource();

    void setURL(jni::JNIEnv&, const jni::String&);
    jni::Local<jni::String> getURL(jni::JNIEnv&);

    void setImage(jni::JNIEnv&, const jni::Object<Bitmap>&);

    void setCoordinates(jni::JNIEnv&, const jni::Object<LatLngQuad>&);

private:
    jni::Local<jni::Object<Source>> createJavaPeer(jni::JNIEnv&);

}; // class ImageSource

} // namespace android
} // namespace mbgl
