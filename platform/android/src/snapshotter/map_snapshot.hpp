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

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/snapshotter/MapSnapshot"; };

    static void registerNative(jni::JNIEnv&);

    static jni::Object<MapSnapshot> New(JNIEnv& env,
                                        PremultipliedImage&& image,
                                        float pixelRatio,
                                        std::vector<std::string> attributions,
                                        bool showLogo,
                                        PointForFn pointForFn);

    MapSnapshot(jni::JNIEnv&) {};
    MapSnapshot(float pixelRatio, PointForFn);
    ~MapSnapshot();

    jni::Object<PointF> pixelForLatLng(jni::JNIEnv&, jni::Object<LatLng>);

private:
    static jni::Class<MapSnapshot> javaClass;

    float pixelRatio;
    mbgl::MapSnapshotter::PointForFn pointForFn;
};

} // namespace android
} // namespace mbgl