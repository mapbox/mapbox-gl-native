#pragma once

#include "source.hpp"
#include <mbgl/style/sources/geojson_source.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class GeoJSONSource : public Source {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/GeoJsonSource"; };

    static jni::Class<GeoJSONSource> javaClass;

    static void registerNative(jni::JNIEnv&);

    GeoJSONSource(jni::JNIEnv&, jni::String, jni::Object<>);

    GeoJSONSource(mbgl::Map&, mbgl::style::GeoJSONSource&);

    ~GeoJSONSource();

    void setGeoJSON(jni::JNIEnv&, jni::Object<>);

    void setURL(jni::JNIEnv&, jni::String);

    jni::jobject* createJavaPeer(jni::JNIEnv&);

}; // class GeoJSONSource

} // namespace android
} // namespace mbgl
