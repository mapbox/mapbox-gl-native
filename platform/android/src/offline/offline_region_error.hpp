#pragma once

#include <mbgl/storage/offline.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class OfflineRegionError {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegionError"; };

    static jni::Object<OfflineRegionError> New(jni::JNIEnv&, mbgl::Response::Error);

    static jni::Class<OfflineRegionError> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
