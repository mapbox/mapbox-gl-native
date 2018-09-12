#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <jni/jni.hpp>

#include <string>

#include "../geometry/lat_lng.hpp"

namespace mbgl {
namespace android {

class Marker : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/annotations/Marker"; };

    static mbgl::Point<double> getPosition(jni::JNIEnv&, const jni::Object<Marker>&);

    static std::string getIconId(jni::JNIEnv&, const jni::Object<Marker>&);

    static void registerNative(jni::JNIEnv&);

};


} // namespace android
} // namespace mbgl