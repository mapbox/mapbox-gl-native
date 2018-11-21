#pragma once

#include <mbgl/map/map_snapshotter.hpp>

#include <jni/jni.hpp>

#include "../geometry/lat_lng.hpp"
#include "../graphics/pointf.hpp"

#include <vector>
#include <string>

namespace mbgl {
namespace android {

class MapSnapshot {
public:

    using PointForFn = mbgl::MapSnapshotter::PointForFn;
    using LatLngForFn = mbgl::MapSnapshotter::LatLngForFn;

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/snapshotter/MapSnapshot"; };

    static void registerNative(jni::JNIEnv&);

    static jni::Local<jni::Object<MapSnapshot>> New(JNIEnv& env,
                                        PremultipliedImage&& image,
                                        float pixelRatio,
                                        std::vector<std::string> attributions,
                                        bool showLogo,
                                        PointForFn pointForFn,
                                        LatLngForFn latLngForFn);

    MapSnapshot(jni::JNIEnv&) {};
    MapSnapshot(float pixelRatio, PointForFn, LatLngForFn);
    ~MapSnapshot();

    jni::Local<jni::Object<PointF>> pixelForLatLng(jni::JNIEnv&, jni::Object<LatLng>&);
    jni::Local<jni::Object<LatLng>> latLngForPixel(jni::JNIEnv&, jni::Object<PointF>&);

private:
    float pixelRatio;
    mbgl::MapSnapshotter::PointForFn pointForFn;
    mbgl::MapSnapshotter::LatLngForFn latLngForFn;
};

} // namespace android
} // namespace mbgl