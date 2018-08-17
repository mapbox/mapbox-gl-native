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
        callback = std::shared_ptr<jni::jobject>(callback_.NewGlobalRef(env_).release().Get(), GenericGlobalRefDeleter()),
        jFileSource = std::shared_ptr<jni::jobject>(jFileSource_.NewGlobalRef(env_).release().Get(), GenericGlobalRefDeleter())
    ](mbgl::expected<mbgl::OfflineRegions, std::exception_ptr> regions) mutable {

        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (regions) {
            OfflineManager::ListOfflineRegionsCallback::onList(
                *env, jni::Object<FileSource>(*jFileSource),
                jni::Object<ListOfflineRegionsCallback>(*callback), std::move(*regions));
        } else {
            OfflineManager::ListOfflineRegionsCallback::onError(
                *env, jni::Object<ListOfflineRegionsCallback>(*callback), regions.error());
        }
    });
}

void OfflineManager::createOfflineRegion(jni::JNIEnv& env_,
                                         jni::Object<FileSource> jFileSource_,
                                         jni::Object<OfflineRegionDefinition> definition_,
                                         jni::Array<jni::jbyte> metadata_,
                                         jni::Object<CreateOfflineRegionCallback> callback_) {
    // Convert
    auto definition = OfflineRegionDefinition::getDefinition(env_, definition_);

    mbgl::OfflineRegionMetadata metadata;
    if (metadata_) {
        metadata = OfflineRegion::metadata(env_, metadata_);
    }

    // Create region
    fileSource.createOfflineRegion(definition, metadata, [
        //Keep a shared ptr to a global reference of the callback and file source so they are not GC'd in the meanwhile
        callback = std::shared_ptr<jni::jobject>(callback_.NewGlobalRef(env_).release().Get(), GenericGlobalRefDeleter()),
        jFileSource = std::shared_ptr<jni::jobject>(jFileSource_.NewGlobalRef(env_).release().Get(), GenericGlobalRefDeleter())
    ](mbgl::expected<mbgl::OfflineRegion, std::exception_ptr> region) mutable {

        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (region) {
            OfflineManager::CreateOfflineRegionCallback::onCreate(
                *env,
                jni::Object<FileSource>(*jFileSource),
                jni::Object<CreateOfflineRegionCallback>(*callback), std::move(*region)
            );
        } else {
            OfflineManager::CreateOfflineRegionCallback::onError(*env, jni::Object<CreateOfflineRegionCallback>(*callback), region.error());
        }
    });
}

void OfflineManager::registerNative(jni::JNIEnv& env) {
    jni::Class<ListOfflineRegionsCallback>::Singleton(env);
    jni::Class<CreateOfflineRegionCallback>::Singleton(env);

    static auto javaClass = jni::Class<OfflineManager>::Singleton(env);

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
    static auto javaClass = jni::Class<OfflineManager::ListOfflineRegionsCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");

    callback.Call(env, method,
        *jni::SeizeLocal(env, jni::Make<jni::String>(env, mbgl::util::toString(error))));
}

void OfflineManager::ListOfflineRegionsCallback::onList(jni::JNIEnv& env,
                                                        jni::Object<FileSource> jFileSource,
                                                        jni::Object<OfflineManager::ListOfflineRegionsCallback> callback,
                                                        mbgl::optional<std::vector<mbgl::OfflineRegion>> regions) {
    static auto javaClass = jni::Class<OfflineManager::ListOfflineRegionsCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::Array<jni::Object<OfflineRegion>>)>(env, "onList");

    std::size_t index = 0;
    auto jregions = jni::SeizeLocal(env, jni::Array<jni::Object<OfflineRegion>>::New(env, regions->size()));
    for (auto& region : *regions) {
        jregions->Set(env, index, *jni::SeizeLocal(env, OfflineRegion::New(env, jFileSource, std::move(region))));
        index++;
    }

    callback.Call(env, method, *jregions);
}

// OfflineManager::CreateOfflineRegionCallback //

void OfflineManager::CreateOfflineRegionCallback::onError(jni::JNIEnv& env,
                                                          jni::Object<OfflineManager::CreateOfflineRegionCallback> callback,
                                                          std::exception_ptr error) {
    static auto javaClass = jni::Class<OfflineManager::CreateOfflineRegionCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");

    callback.Call(env, method,
        *jni::SeizeLocal(env, jni::Make<jni::String>(env, mbgl::util::toString(error))));
}

void OfflineManager::CreateOfflineRegionCallback::onCreate(jni::JNIEnv& env,
                                                        jni::Object<FileSource> jFileSource,
                                                        jni::Object<OfflineManager::CreateOfflineRegionCallback> callback,
                                                        mbgl::optional<mbgl::OfflineRegion> region) {
    static auto javaClass = jni::Class<OfflineManager::CreateOfflineRegionCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::Object<OfflineRegion>)>(env, "onCreate");

    callback.Call(env, method,
        *jni::SeizeLocal(env, OfflineRegion::New(env, jFileSource, std::move(*region))));
}

} // namespace android
} // namespace mbgl
