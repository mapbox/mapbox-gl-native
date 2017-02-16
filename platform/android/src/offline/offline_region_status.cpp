#include "offline_region_status.hpp"

namespace mbgl {
namespace android {

jni::Object<OfflineRegionStatus> OfflineRegionStatus::New(jni::JNIEnv& env, mbgl::OfflineRegionStatus status) {

    // Convert to jint
    jint downloadState;
    switch(status.downloadState) {
        case mbgl::OfflineRegionDownloadState::Inactive:
            downloadState = 0;
            break;
        case mbgl::OfflineRegionDownloadState::Active:
            downloadState = 1;
            break;
    }

    // Create java object
    static auto constructor = javaClass.GetConstructor<jint, jlong, jlong, jlong, jlong, jlong, jboolean>(env);
    return javaClass.New(env, constructor,
        downloadState,
        jlong(status.completedResourceCount),
        jlong(status.completedResourceSize),
        jlong(status.completedTileCount),
        jlong(status.completedTileSize),
        jlong(status.requiredResourceCount),
        jboolean(status.requiredResourceCountIsPrecise)
    );
}

jni::Class<OfflineRegionStatus> OfflineRegionStatus::javaClass;

void OfflineRegionStatus::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<OfflineRegionStatus>::Find(env).NewGlobalRef(env).release();
}

} // namespace android
} // namespace mbgl
