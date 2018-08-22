#pragma once

#include <mbgl/util/feature.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {
namespace geojson {

class Feature : public jni::ObjectTag {
public:
    static constexpr auto Name() { return "com/mapbox/geojson/Feature"; };

    static mbgl::Feature convert(jni::JNIEnv&, jni::Object<Feature>);
    static jni::Array<jni::Object<Feature>> convert(jni::JNIEnv&, const std::vector<mbgl::Feature>&);

    static void registerNative(jni::JNIEnv&);
};

} // namespace geojson
} // namespace android
} // namespace mbgl