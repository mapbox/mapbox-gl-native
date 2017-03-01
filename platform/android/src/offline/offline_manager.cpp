#include "offline_manager.hpp"

#include <mbgl/util/string.hpp>

#include "../attach_env.hpp"
#include "../jni/generic_global_ref_deleter.hpp"

namespace mbgl {
namespace android {

// OfflineManager //

OfflineManager::OfflineManager(jni::JNIEnv& env, jni::Object<FileSource> jFileSource)
    : fileSource(mbgl::android::FileSource::getDefaultFileSource(env, jFileSource)) {
}

OfflineManager::~OfflineManager() {}

void OfflineManager::setOfflineMapboxTileCountLimit(jni::JNIEnv&, jni::jlong limit) {
    fileSource.setOfflineMapboxTileCountLimit(limit);
}

void OfflineManager::listOfflineRegions(jni::JNIEnv& env_, jni::Object<FileSource> jFileSource_, jni::Object<ListOfflineRegionsCallback> callback_) {
    // list regions
    fileSource.listOfflineRegions([
        //Keep a shared ptr to a global reference of the callback and file source so they are not GC'd in the meanwhile
        callback = std::shared_ptr<jni::jobject>(callback_.NewGlobalRef(env_).release()->Get(), GenericGlobalRefDeleter()),
        jFileSource = std::shared_ptr<jni::jobject>(jFileSource_.NewGlobalRef(env_).release()->Get(), GenericGlobalRefDeleter())
    ](std::exception_ptr error, mbgl::optional<std::vector<mbgl::OfflineRegion>> regions) mutable {

        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (error) {
            OfflineManager::ListOfflineRegionsCallback::onError(*env, jni::Object<ListOfflineRegionsCallback>(*callback), error);
        } else if (regions) {
            OfflineManager::ListOfflineRegionsCallback::onList(*env, jni::Object<FileSource>(*jFileSource), jni::Object<ListOfflineRegionsCallback>(*callback), std::move(regions));
        }
    });
}

void OfflineManager::createOfflineRegion(jni::JNIEnv& env_,
                                         jni::Object<FileSource> jFileSource_,
                                         jni::Object<OfflineRegionDefinition> definition_,
                                         jni::Array<jni::jbyte> metadata_,
                                         jni::Object<CreateOfflineRegionCallback> callback_) {
    // Convert

    // XXX hardcoded cast for now as we only support OfflineTilePyramidRegionDefinition
    auto definition = OfflineTilePyramidRegionDefinition::getDefinition(env_, jni::Object<OfflineTilePyramidRegionDefinition>(*definition_));

    mbgl::OfflineRegionMetadata metadata;
    if (metadata_) {
        metadata = OfflineRegion::metadata(env_, metadata_);
    }

    // Create region
    fileSource.createOfflineRegion(definition, metadata, [
        //Keep a shared ptr to a global reference of the callback and file source so they are not GC'd in the meanwhile
        callback = std::shared_ptr<jni::jobject>(callback_.NewGlobalRef(env_).release()->Get(), GenericGlobalRefDeleter()),
        jFileSource = std::shared_ptr<jni::jobject>(jFileSource_.NewGlobalRef(env_).release()->Get(), GenericGlobalRefDeleter())
    ](std::exception_ptr error, mbgl::optional<mbgl::OfflineRegion> region) mutable {

        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (error) {
            OfflineManager::CreateOfflineRegionCallback::onError(*env, jni::Object<CreateOfflineRegionCallback>(*callback), error);
        } else if (region) {
            OfflineManager::CreateOfflineRegionCallback::onCreate(
                *env,
                jni::Object<FileSource>(*jFileSource),
                jni::Object<CreateOfflineRegionCallback>(*callback), std::move(region)
            );
        }
    });
}

jni::Class<OfflineManager> OfflineManager::javaClass;

void OfflineManager::registerNative(jni::JNIEnv& env) {
    OfflineManager::ListOfflineRegionsCallback::registerNative(env);
    OfflineManager::CreateOfflineRegionCallback::registerNative(env);

    javaClass = *jni::Class<OfflineManager>::Find(env).NewGlobalRef(env).release();

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    jni::RegisterNativePeer<OfflineManager>( env, javaClass, "nativePtr",
        std::make_unique<OfflineManager, JNIEnv&, jni::Object<FileSource>>,
        "initialize",
        "finalize",
        METHOD(&OfflineManager::setOfflineMapboxTileCountLimit, "setOfflineMapboxTileCountLimit"),
        METHOD(&OfflineManager::listOfflineRegions, "listOfflineRegions"),
        METHOD(&OfflineManager::createOfflineRegion, "createOfflineRegion"));
}

// OfflineManager::ListOfflineRegionsCallback //

void OfflineManager::ListOfflineRegionsCallback::onError(jni::JNIEnv& env,
                                                          jni::Object<OfflineManager::ListOfflineRegionsCallback> callback,
                                                          std::exception_ptr error) {
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");
    std::string message = mbgl::util::toString(error);
    callback.Call(env, method, jni::Make<jni::String>(env, message));
}

void OfflineManager::ListOfflineRegionsCallback::onList(jni::JNIEnv& env,
                                                        jni::Object<FileSource> jFileSource,
                                                        jni::Object<OfflineManager::ListOfflineRegionsCallback> callback,
                                                        mbgl::optional<std::vector<mbgl::OfflineRegion>> regions) {
    //Convert the regions to java peer objects
    std::size_t index = 0;
    auto jregions = jni::Array<jni::Object<OfflineRegion>>::New(env, regions->size(), OfflineRegion::javaClass);
    for (auto& region : *regions) {
        auto jregion = OfflineRegion::New(env, jFileSource, std::move(region));
        jregions.Set(env, index, jregion);
        jni::DeleteLocalRef(env, jregion);
        index++;
    }

    // Trigger callback
    static auto method = javaClass.GetMethod<void (jni::Array<jni::Object<OfflineRegion>>)>(env, "onList");
    callback.Call(env, method, jregions);
    jni::DeleteLocalRef(env, jregions);
}

jni::Class<OfflineManager::ListOfflineRegionsCallback> OfflineManager::ListOfflineRegionsCallback::javaClass;

void OfflineManager::ListOfflineRegionsCallback::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<OfflineManager::ListOfflineRegionsCallback>::Find(env).NewGlobalRef(env).release();
}

// OfflineManager::CreateOfflineRegionCallback //

void OfflineManager::CreateOfflineRegionCallback::onError(jni::JNIEnv& env,
                                                          jni::Object<OfflineManager::CreateOfflineRegionCallback> callback,
                                                          std::exception_ptr error) {
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");
    std::string message = mbgl::util::toString(error);
    callback.Call(env, method, jni::Make<jni::String>(env, message));
}

void OfflineManager::CreateOfflineRegionCallback::onCreate(jni::JNIEnv& env,
                                                        jni::Object<FileSource> jFileSource,
                                                        jni::Object<OfflineManager::CreateOfflineRegionCallback> callback,
                                                        mbgl::optional<mbgl::OfflineRegion> region) {
    //Convert the region to java peer object
    auto jregion = OfflineRegion::New(env, jFileSource, std::move(*region));

    // Trigger callback
    static auto method = javaClass.GetMethod<void (jni::Object<OfflineRegion>)>(env, "onCreate");
    callback.Call(env, method, jregion);
    jni::DeleteLocalRef(env, jregion);
}

jni::Class<OfflineManager::CreateOfflineRegionCallback> OfflineManager::CreateOfflineRegionCallback::javaClass;

void OfflineManager::CreateOfflineRegionCallback::registerNative(jni::JNIEnv& env) {
    javaClass = *jni::Class<OfflineManager::CreateOfflineRegionCallback>::Find(env).NewGlobalRef(env).release();
}

} // namespace android
} // namespace mbgl
