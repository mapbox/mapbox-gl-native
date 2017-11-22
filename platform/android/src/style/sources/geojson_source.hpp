#pragma once

#include "source.hpp"
#include <mbgl/style/sources/geojson_source.hpp>
#include "../../geojson/geometry.hpp"
#include "../../geojson/feature.hpp"
#include "../../geojson/feature_collection.hpp"
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class GeoJSONSource : public Source {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/GeoJsonSource"; };

    static jni::Class<GeoJSONSource> javaClass;

    static void registerNative(jni::JNIEnv&);

    GeoJSONSource(jni::JNIEnv&, jni::String, jni::Object<>);

    GeoJSONSource(jni::JNIEnv&, mbgl::style::Source&, AndroidRendererFrontend&);

    ~GeoJSONSource();

    void setGeoJSONString(jni::JNIEnv&, jni::String);

    void setFeatureCollection(jni::JNIEnv&, jni::Object<geojson::FeatureCollection>);

    void setFeature(jni::JNIEnv&, jni::Object<geojson::Feature>);

    void setGeometry(jni::JNIEnv&, jni::Object<geojson::Geometry>);

    void setURL(jni::JNIEnv&, jni::String);

    jni::Array<jni::Object<geojson::Feature>> querySourceFeatures(jni::JNIEnv&,
                                                                  jni::Array<jni::Object<>> jfilter);

    jni::String getURL(jni::JNIEnv&);

private:
    jni::Object<Source> createJavaPeer(jni::JNIEnv&);

}; // class GeoJSONSource

} // namespace android
} // namespace mbgl
