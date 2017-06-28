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

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/ImageSource"; };

    static jni::Class<ImageSource> javaClass;

    static void registerNative(jni::JNIEnv&);

    ImageSource(jni::JNIEnv&, jni::String, jni::Object<LatLngQuad>);

    ImageSource(mbgl::style::ImageSource&);

    ~ImageSource();

    void setURL(jni::JNIEnv&, jni::String);
    jni::String getURL(jni::JNIEnv&);

    void setImage(jni::JNIEnv&, jni::Object<Bitmap>);

    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class ImageSource

} // namespace android
} // namespace mbgl
