#pragma once

#include "source.hpp"
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/tile/tile_id.hpp>
#include "../../geojson/geometry.hpp"
#include "../../geojson/feature.hpp"
#include "../../geojson/feature_collection.hpp"
#include "../../geometry/lat_lng_bounds.hpp"
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class CustomGeometrySource : public Source {
public:
    using SuperTag = Source;
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/CustomGeometrySource"; };

    static void registerNative(jni::JNIEnv&);

    CustomGeometrySource(jni::JNIEnv&, const jni::String&, const jni::Object<>&);
    CustomGeometrySource(jni::JNIEnv&, mbgl::style::Source&, AndroidRendererFrontend&);
    ~CustomGeometrySource();

    bool removeFromMap(JNIEnv&, const jni::Object<Source>&, mbgl::Map&) override;
    void addToMap(JNIEnv&, const jni::Object<Source>&, mbgl::Map&, AndroidRendererFrontend&) override;

    void fetchTile(const mbgl::CanonicalTileID& tileID);
    void cancelTile(const mbgl::CanonicalTileID& tileID);
    bool isCancelled(jni::jint z, jni::jint x, jni::jint y);
    void startThreads();
    void releaseThreads();

private:
    void setTileData(jni::JNIEnv& env, jni::jint z, jni::jint x, jni::jint y, const jni::Object<geojson::FeatureCollection>& jf);

    void invalidateTile(jni::JNIEnv& env, jni::jint z, jni::jint x, jni::jint y);
    void invalidateBounds(jni::JNIEnv& env, const jni::Object<LatLngBounds>& bounds);

    jni::Local<jni::Array<jni::Object<geojson::Feature>>> querySourceFeatures(jni::JNIEnv&,
                                                                  const jni::Array<jni::Object<>>& );

    jni::Local<jni::Object<Source>> createJavaPeer(jni::JNIEnv&);

}; // class CustomGeometrySource

} // namespace android
} // namespace mbgl
