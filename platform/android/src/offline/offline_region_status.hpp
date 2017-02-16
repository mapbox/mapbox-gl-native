#pragma once

#include <mbgl/storage/offline.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class OfflineRegionStatus {
public:
    static constexpr auto Name() { return "com/mapbox/mapboxsdk/offline/OfflineRegionStatus"; };

    static jni::Object<OfflineRegionStatus> New(jni::JNIEnv&, mbgl::OfflineRegionStatus status);

    static jni::Class<OfflineRegionStatus> javaClass;

    static void registerNative(jni::JNIEnv&);
};

} // namespace android
} // namespace mbgl
