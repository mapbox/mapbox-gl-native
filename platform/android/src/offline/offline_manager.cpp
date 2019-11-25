#include "offline_manager.hpp"

#include <mbgl/util/string.hpp>

#include "../attach_env.hpp"

namespace mbgl {
namespace android {

namespace {
// Reattach, the callback comes from a different thread
void handleException(std::exception_ptr exception,
                     const jni::Object<OfflineManager::FileSourceCallback>& callback,
                     android::UniqueEnv env = android::AttachEnv()) {
    if (exception) {
        OfflineManager::FileSourceCallback::onError(
            *env, callback, jni::Make<jni::String>(*env, mbgl::util::toString(exception)));
    } else {
        OfflineManager::FileSourceCallback::onSuccess(*env, callback);
    }
}
} // namespace

// OfflineManager //
OfflineManager::OfflineManager(jni::JNIEnv& env, const jni::Object<FileSource>& jFileSource)
    : fileSource(std::static_pointer_cast<DefaultFileSource>(mbgl::FileSource::getSharedFileSource(FileSource::getSharedResourceOptions(env, jFileSource)))) {}

OfflineManager::~OfflineManager() {}

void OfflineManager::setOfflineMapboxTileCountLimit(jni::JNIEnv&, jni::jlong limit) {
    fileSource->setOfflineMapboxTileCountLimit(limit);
}

void OfflineManager::listOfflineRegions(jni::JNIEnv& env_, const jni::Object<FileSource>& jFileSource_, const jni::Object<ListOfflineRegionsCallback>& callback_) {
    auto globalCallback = jni::NewGlobal<jni::EnvAttachingDeleter>(env_, callback_);
    auto globalFilesource = jni::NewGlobal<jni::EnvAttachingDeleter>(env_, jFileSource_);

    fileSource->listOfflineRegions([
        //Keep a shared ptr to a global reference of the callback and file source so they are not GC'd in the meanwhile
        callback = std::make_shared<decltype(globalCallback)>(std::move(globalCallback)),
        jFileSource = std::make_shared<decltype(globalFilesource)>(std::move(globalFilesource))
    ](mbgl::expected<mbgl::OfflineRegions, std::exception_ptr> regions) mutable {

        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (regions) {
            OfflineManager::ListOfflineRegionsCallback::onList(
                *env, *jFileSource, *callback, *regions);
        } else {
            OfflineManager::ListOfflineRegionsCallback::onError(
                *env, *callback, regions.error());
        }
    });
}

void OfflineManager::createOfflineRegion(jni::JNIEnv& env_,
                                         const jni::Object<FileSource>& jFileSource_,
                                         const jni::Object<OfflineRegionDefinition>& definition_,
                                         const jni::Array<jni::jbyte>& metadata_,
                                         const jni::Object<CreateOfflineRegionCallback>& callback_) {
    // Convert
    auto definition = OfflineRegionDefinition::getDefinition(env_, definition_);

    mbgl::OfflineRegionMetadata metadata;
    if (metadata_) {
        metadata = OfflineRegion::metadata(env_, metadata_);
    }

    auto globalCallback = jni::NewGlobal<jni::EnvAttachingDeleter>(env_, callback_);
    auto globalFilesource = jni::NewGlobal<jni::EnvAttachingDeleter>(env_, jFileSource_);

    // Create region
    fileSource->createOfflineRegion(definition, metadata, [
        //Keep a shared ptr to a global reference of the callback and file source so they are not GC'd in the meanwhile
        callback = std::make_shared<decltype(globalCallback)>(std::move(globalCallback)),
        jFileSource = std::make_shared<decltype(globalFilesource)>(std::move(globalFilesource))
    ](mbgl::expected<mbgl::OfflineRegion, std::exception_ptr> region) mutable {

        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (region) {
            OfflineManager::CreateOfflineRegionCallback::onCreate(
                *env, *jFileSource, *callback, *region
            );
        } else {
            OfflineManager::CreateOfflineRegionCallback::onError(
                *env, *callback, region.error());
        }
    });
}

void OfflineManager::mergeOfflineRegions(jni::JNIEnv& env_, const jni::Object<FileSource>& jFileSource_,
                                         const jni::String& jString_,
                                         const jni::Object<MergeOfflineRegionsCallback>& callback_) {
    auto globalCallback = jni::NewGlobal<jni::EnvAttachingDeleter>(env_, callback_);
    auto globalFilesource = jni::NewGlobal<jni::EnvAttachingDeleter>(env_, jFileSource_);

    auto path = jni::Make<std::string>(env_, jString_);
    fileSource->mergeOfflineRegions(path, [
        //Keep a shared ptr to a global reference of the callback and file source so they are not GC'd in the meanwhile
        callback = std::make_shared<decltype(globalCallback)>(std::move(globalCallback)),
        jFileSource = std::make_shared<decltype(globalFilesource)>(std::move(globalFilesource))
    ](mbgl::expected<mbgl::OfflineRegions, std::exception_ptr> regions) mutable {

        // Reattach, the callback comes from a different thread
        android::UniqueEnv env = android::AttachEnv();

        if (regions) {
            OfflineManager::MergeOfflineRegionsCallback::onMerge(
                *env, *jFileSource, *callback, *regions);
        } else {
            OfflineManager::MergeOfflineRegionsCallback::onError(
                *env, *callback, regions.error());
        }
    });
}

void OfflineManager::resetDatabase(jni::JNIEnv& env_, const jni::Object<FileSourceCallback>& callback_) {
    auto globalCallback = jni::NewGlobal<jni::EnvAttachingDeleter>(env_, callback_);

    fileSource->resetDatabase(
        [
            // Keep a shared ptr to a global reference of the callback so they are not GC'd in the meanwhile
            callback = std::make_shared<decltype(globalCallback)>(std::move(globalCallback))](
            std::exception_ptr exception) mutable { handleException(exception, *callback); });
}

void OfflineManager::packDatabase(jni::JNIEnv& env_, const jni::Object<FileSourceCallback>& callback_) {
    auto globalCallback = jni::NewGlobal<jni::EnvAttachingDeleter>(env_, callback_);

    fileSource->packDatabase(
        [
            // Keep a shared ptr to a global reference of the callback so they are not GC'd in the meanwhile
            callback = std::make_shared<decltype(globalCallback)>(std::move(globalCallback))](
            std::exception_ptr exception) mutable { handleException(exception, *callback); });
}

void OfflineManager::invalidateAmbientCache(jni::JNIEnv& env_, const jni::Object<FileSourceCallback>& callback_) {
    auto globalCallback = jni::NewGlobal<jni::EnvAttachingDeleter>(env_, callback_);

    fileSource->invalidateAmbientCache(
        [
            // Keep a shared ptr to a global reference of the callback so they are not GC'd in the meanwhile
            callback = std::make_shared<decltype(globalCallback)>(std::move(globalCallback))](
            std::exception_ptr exception) mutable { handleException(exception, *callback); });
}

void OfflineManager::clearAmbientCache(jni::JNIEnv& env_, const jni::Object<FileSourceCallback>& callback_) {
    auto globalCallback = jni::NewGlobal<jni::EnvAttachingDeleter>(env_, callback_);

    fileSource->clearAmbientCache(
        [
            // Keep a shared ptr to a global reference of the callback so they are not GC'd in the meanwhile
            callback = std::make_shared<decltype(globalCallback)>(std::move(globalCallback))](
            std::exception_ptr exception) mutable { handleException(exception, *callback); });
}

void OfflineManager::setMaximumAmbientCacheSize(jni::JNIEnv& env_, const jni::jlong size_, const jni::Object<FileSourceCallback>& callback_) {
    auto globalCallback = jni::NewGlobal<jni::EnvAttachingDeleter>(env_, callback_);

    fileSource->setMaximumAmbientCacheSize(
        size_,
        [
            // Keep a shared ptr to a global reference of the callback so they are not GC'd in the meanwhile
            callback = std::make_shared<decltype(globalCallback)>(std::move(globalCallback))](
            std::exception_ptr exception) mutable { handleException(exception, *callback); });
}

void OfflineManager::runPackDatabaseAutomatically(jni::JNIEnv&, jboolean autopack) {
    fileSource->runPackDatabaseAutomatically(autopack);
}

// FileSource::FileSourceCallback //

void OfflineManager::FileSourceCallback::onSuccess(jni::JNIEnv& env,
                                               const jni::Object<OfflineManager::FileSourceCallback>& callback) {
    static auto& javaClass = jni::Class<OfflineManager::FileSourceCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void ()>(env, "onSuccess");
    callback.Call(env, method);
}

void OfflineManager::FileSourceCallback::onError(jni::JNIEnv& env,
                                             const jni::Object<OfflineManager::FileSourceCallback>& callback,
                                             const jni::String& message) {
    static auto& javaClass = jni::Class<OfflineManager::FileSourceCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");
    callback.Call(env, method, message);
}

void OfflineManager::registerNative(jni::JNIEnv& env) {
    jni::Class<ListOfflineRegionsCallback>::Singleton(env);
    jni::Class<CreateOfflineRegionCallback>::Singleton(env);
    jni::Class<MergeOfflineRegionsCallback>::Singleton(env);
    jni::Class<FileSourceCallback>::Singleton(env);

    static auto& javaClass = jni::Class<OfflineManager>::Singleton(env);

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    jni::RegisterNativePeer<OfflineManager>(
        env,
        javaClass,
        "nativePtr",
        jni::MakePeer<OfflineManager, const jni::Object<FileSource>&>,
        "initialize",
        "finalize",
        METHOD(&OfflineManager::setOfflineMapboxTileCountLimit, "setOfflineMapboxTileCountLimit"),
        METHOD(&OfflineManager::listOfflineRegions, "listOfflineRegions"),
        METHOD(&OfflineManager::createOfflineRegion, "createOfflineRegion"),
        METHOD(&OfflineManager::mergeOfflineRegions, "mergeOfflineRegions"),
        METHOD(&OfflineManager::resetDatabase, "nativeResetDatabase"),
        METHOD(&OfflineManager::packDatabase, "nativePackDatabase"),
        METHOD(&OfflineManager::invalidateAmbientCache, "nativeInvalidateAmbientCache"),
        METHOD(&OfflineManager::clearAmbientCache, "nativeClearAmbientCache"),
        METHOD(&OfflineManager::setMaximumAmbientCacheSize, "nativeSetMaximumAmbientCacheSize"),
        METHOD(&OfflineManager::runPackDatabaseAutomatically, "runPackDatabaseAutomatically"),
        METHOD(&OfflineManager::putResourceWithUrl, "putResourceWithUrl"));
}

// OfflineManager::ListOfflineRegionsCallback //

void OfflineManager::ListOfflineRegionsCallback::onError(jni::JNIEnv& env,
                                                          const jni::Object<OfflineManager::ListOfflineRegionsCallback>& callback,
                                                          std::exception_ptr error) {
    static auto& javaClass = jni::Class<OfflineManager::ListOfflineRegionsCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");

    callback.Call(env, method, jni::Make<jni::String>(env, mbgl::util::toString(error)));
}

void OfflineManager::ListOfflineRegionsCallback::onList(jni::JNIEnv& env,
                                                        const jni::Object<FileSource>& jFileSource,
                                                        const jni::Object<OfflineManager::ListOfflineRegionsCallback>& callback,
                                                        mbgl::OfflineRegions& regions) {
    static auto& javaClass = jni::Class<OfflineManager::ListOfflineRegionsCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::Array<jni::Object<OfflineRegion>>)>(env, "onList");

    std::size_t index = 0;
    auto jregions = jni::Array<jni::Object<OfflineRegion>>::New(env, regions.size());
    for (auto& region : regions) {
        jregions.Set(env, index, OfflineRegion::New(env, jFileSource, std::move(region)));
        index++;
    }

    callback.Call(env, method, jregions);
}

// OfflineManager::CreateOfflineRegionCallback //

void OfflineManager::CreateOfflineRegionCallback::onError(jni::JNIEnv& env,
                                                          const jni::Object<OfflineManager::CreateOfflineRegionCallback>& callback,
                                                          std::exception_ptr error) {
    static auto& javaClass = jni::Class<OfflineManager::CreateOfflineRegionCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");

    callback.Call(env, method, jni::Make<jni::String>(env, mbgl::util::toString(error)));
}

void OfflineManager::CreateOfflineRegionCallback::onCreate(jni::JNIEnv& env,
                                                        const jni::Object<FileSource>& jFileSource,
                                                        const jni::Object<OfflineManager::CreateOfflineRegionCallback>& callback,
                                                        mbgl::OfflineRegion& region) {
    static auto& javaClass = jni::Class<OfflineManager::CreateOfflineRegionCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::Object<OfflineRegion>)>(env, "onCreate");

    callback.Call(env, method, OfflineRegion::New(env, jFileSource, std::move(region)));
}

// OfflineManager::MergeOfflineRegionsCallback //

void OfflineManager::MergeOfflineRegionsCallback::onError(jni::JNIEnv& env,
                                                          const jni::Object<OfflineManager::MergeOfflineRegionsCallback>& callback,
                                                          std::exception_ptr error) {
    static auto& javaClass = jni::Class<OfflineManager::MergeOfflineRegionsCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");

    callback.Call(env, method, jni::Make<jni::String>(env, mbgl::util::toString(error)));
}

void OfflineManager::MergeOfflineRegionsCallback::onMerge(jni::JNIEnv& env,
                                                          const jni::Object<FileSource>& jFileSource,
                                                          const jni::Object<MergeOfflineRegionsCallback>& callback,
                                                          mbgl::OfflineRegions& regions) {
    static auto& javaClass = jni::Class<OfflineManager::MergeOfflineRegionsCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::Array<jni::Object<OfflineRegion>>)>(env, "onMerge");

    std::size_t index = 0;
    auto jregions = jni::Array<jni::Object<OfflineRegion>>::New(env, regions.size());
    for (auto& region : regions) {
        jregions.Set(env, index, OfflineRegion::New(env, jFileSource, std::move(region)));
        index++;
    }

    callback.Call(env, method, jregions);
}

void OfflineManager::putResourceWithUrl(jni::JNIEnv& env,
                                        const jni::String& url_,
                                        const jni::Array<jni::jbyte>& arr,
                                        jlong modified,
                                        jlong expires,
                                        const jni::String& eTag_,
                                        jboolean mustRevalidate) {
    auto url =  jni::Make<std::string>(env, url_);
    auto data = std::make_shared<std::string>(arr.Length(env), char());
    jni::GetArrayRegion(env, *arr, 0, data->size(), reinterpret_cast<jbyte*>(&(*data)[0]));
    mbgl::Resource resource(mbgl::Resource::Kind::Unknown, url);
    mbgl::Response response;
    response.data = data;
    response.mustRevalidate = mustRevalidate;
    if (eTag_) {
        response.etag = jni::Make<std::string>(env, eTag_);
    }
    if (modified > 0) {
        response.modified = Timestamp(mbgl::Seconds(modified));
    }
    if (expires > 0) {
        response.expires = Timestamp(mbgl::Seconds(expires));
    }

    fileSource->put(resource, response);
}

} // namespace android
} // namespace mbgl
