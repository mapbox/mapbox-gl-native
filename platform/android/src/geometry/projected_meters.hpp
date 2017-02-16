#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/geometry.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class ProjectedMeters : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/geometry/ProjectedMeters"; };

    static jni::Object<ProjectedMeters> New(jni::JNIEnv&, double, double);

    static jni::Class<ProjectedMeters> javaClass;

    static void registerNative(jni::JNIEnv&);

};


} // namespace android
} // namespace mbgl