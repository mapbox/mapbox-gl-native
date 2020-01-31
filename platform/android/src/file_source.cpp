#include "file_source.hpp"

#include "attach_env.hpp"
#include "mapbox.hpp"

#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/storage/resource_transform.hpp>
#include <mbgl/util/logging.hpp>

#include <mbgl/storage/sqlite3.hpp>

#include "asset_manager_file_source.hpp"

namespace mbgl {

std::shared_ptr<FileSource> FileSource::createPlatformFileSource(const ResourceOptions& options) {
    auto env{android::AttachEnv()};
    std::shared_ptr<DefaultFileSource> fileSource;
    if (android::Mapbox::hasInstance(*env)) {
        auto assetManager = android::Mapbox::getAssetManager(*env);
        fileSource = std::make_shared<DefaultFileSource>(options.cachePath(),
                                                         std::make_unique<AssetManagerFileSource>(*env, assetManager));
    } else {
        fileSource = std::make_shared<DefaultFileSource>(options.cachePath(), options.assetPath());
    }
    fileSource->setAccessToken(options.accessToken());
    return fileSource;
}

namespace android {

// FileSource //

FileSource::FileSource(jni::JNIEnv& _env, const jni::String& accessToken, const jni::String& _cachePath) {
    std::string path = jni::Make<std::string>(_env, _cachePath);
    mapbox::sqlite::setTempPath(path);

    resourceOptions.withAccessToken(accessToken ? jni::Make<std::string>(_env, accessToken) : "")
        .withCachePath(path + DATABASE_FILE);

    // Create a core default file source
    fileSource = std::static_pointer_cast<mbgl::DefaultFileSource>(mbgl::FileSource::getSharedFileSource(resourceOptions));
}

FileSource::~FileSource() {
}

jni::Local<jni::String> FileSource::getAccessToken(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, fileSource->getAccessToken());
}

void FileSource::setAccessToken(jni::JNIEnv& env, const jni::String& token) {
    fileSource->setAccessToken(token ? jni::Make<std::string>(env, token) : "");
}

void FileSource::setAPIBaseUrl(jni::JNIEnv& env, const jni::String& url) {
    fileSource->setAPIBaseURL(jni::Make<std::string>(env, url));
}

void FileSource::setResourceTransform(jni::JNIEnv& env, const jni::Object<FileSource::ResourceTransformCallback>& transformCallback) {
    if (transformCallback) {
        auto global = jni::NewGlobal<jni::EnvAttachingDeleter>(env, transformCallback);
        resourceTransform = std::make_unique<Actor<ResourceTransform>>(
            *Scheduler::GetCurrent(),
            // Capture the ResourceTransformCallback object as a managed global into
            // the lambda. It is released automatically when we're setting a new ResourceTransform in
            // a subsequent call.
            // Note: we're converting it to shared_ptr because this lambda is converted to a std::function,
            // which requires copyability of its captured variables.
            [callback = std::make_shared<decltype(global)>(std::move(global))](mbgl::Resource::Kind kind,
                                                                               const std::string& url_) {
                android::UniqueEnv _env = android::AttachEnv();
                return FileSource::ResourceTransformCallback::onURL(*_env, *callback, int(kind), url_);
            });
        fileSource->setResourceTransform(resourceTransform->self());
    } else {
        // Reset the callback
        resourceTransform.reset();
        fileSource->setResourceTransform({});
    }
}

void FileSource::setResourceCachePath(jni::JNIEnv& env, const jni::String& path,
                                      const jni::Object<FileSource::ResourcesCachePathChangeCallback>& _callback) {
    if (pathChangeCallback) {
        FileSource::ResourcesCachePathChangeCallback::onError(env, _callback, jni::Make<jni::String>(env, "Another resources cache path change is in progress"));
        return;
    }

    std::string newPath = jni::Make<std::string>(env, path);
    mapbox::sqlite::setTempPath(newPath);

    auto global = jni::NewGlobal<jni::EnvAttachingDeleter>(env, _callback);
    pathChangeCallback = std::make_unique<Actor<PathChangeCallback>>(*Scheduler::GetCurrent(),
            [this, callback = std::make_shared<decltype(global)>(std::move(global)), newPath] {
                android::UniqueEnv _env = android::AttachEnv();
                FileSource::ResourcesCachePathChangeCallback::onSuccess(*_env, *callback, jni::Make<jni::String>(*_env, newPath));
                pathChangeCallback.reset();
            });

    fileSource->setResourceCachePath(newPath + DATABASE_FILE, pathChangeCallback->self());
}

void FileSource::resume(jni::JNIEnv&) {
    if (!activationCounter) {
        activationCounter = optional<int>(1) ;
        return;
    }

    activationCounter.value()++;
    if (activationCounter == 1) {
        fileSource->resume();
    }
}

void FileSource::pause(jni::JNIEnv&) {
    if (activationCounter) {
        activationCounter.value()--;
        if (activationCounter == 0) {
            fileSource->pause();
        }
    }
}

jni::jboolean FileSource::isResumed(jni::JNIEnv&) {
    if (activationCounter) {
       return  (jboolean) (activationCounter > 0);
    }
    return (jboolean) false;
}

FileSource* FileSource::getNativePeer(jni::JNIEnv& env, const jni::Object<FileSource>& jFileSource) {
    static auto& javaClass = jni::Class<FileSource>::Singleton(env);
    static auto field = javaClass.GetField<jlong>(env, "nativePtr");
    return reinterpret_cast<FileSource *>(jFileSource.Get(env, field));
}

mbgl::ResourceOptions FileSource::getSharedResourceOptions(jni::JNIEnv& env, const jni::Object<FileSource>& jFileSource) {
    FileSource* fileSource = FileSource::getNativePeer(env, jFileSource);
    assert(fileSource != nullptr);
    return fileSource->resourceOptions.clone();
}

// FileSource::ResourcesCachePathChangeCallback //

void FileSource::ResourcesCachePathChangeCallback::onSuccess(jni::JNIEnv& env,
                                                             const jni::Object<FileSource::ResourcesCachePathChangeCallback>& callback,
                                                             const jni::String& path) {
    static auto& javaClass = jni::Class<FileSource::ResourcesCachePathChangeCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onSuccess");

    callback.Call(env, method, path);
}

void FileSource::ResourcesCachePathChangeCallback::onError(jni::JNIEnv& env,
                                                             const jni::Object<FileSource::ResourcesCachePathChangeCallback>& callback,
                                                             const jni::String& message) {
    static auto& javaClass = jni::Class<FileSource::ResourcesCachePathChangeCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<void (jni::String)>(env, "onError");

    callback.Call(env, method, message);
}

void FileSource::registerNative(jni::JNIEnv& env) {
    // Ensure the classes are cached. If they're requested for the
    // first time on a background thread, Android's class loader heuristics will fail.
    // https://developer.android.com/training/articles/perf-jni#faq_FindClass
    jni::Class<ResourceTransformCallback>::Singleton(env);
    jni::Class<ResourcesCachePathChangeCallback>::Singleton(env);

    static auto& javaClass = jni::Class<FileSource>::Singleton(env);

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<FileSource>(env,
                                        javaClass,
                                        "nativePtr",
                                        jni::MakePeer<FileSource, const jni::String&, const jni::String&>,
                                        "initialize",
                                        "finalize",
                                        METHOD(&FileSource::getAccessToken, "getAccessToken"),
                                        METHOD(&FileSource::setAccessToken, "setAccessToken"),
                                        METHOD(&FileSource::setAPIBaseUrl, "setApiBaseUrl"),
                                        METHOD(&FileSource::setResourceTransform, "setResourceTransform"),
                                        METHOD(&FileSource::setResourceCachePath, "setResourceCachePath"),
                                        METHOD(&FileSource::resume, "activate"),
                                        METHOD(&FileSource::pause, "deactivate"),
                                        METHOD(&FileSource::isResumed, "isActivated"));
}


// FileSource::ResourceTransformCallback //

std::string FileSource::ResourceTransformCallback::onURL(jni::JNIEnv& env, const jni::Object<FileSource::ResourceTransformCallback>& callback, int kind, std::string url_) {
    static auto& javaClass = jni::Class<FileSource::ResourceTransformCallback>::Singleton(env);
    static auto method = javaClass.GetMethod<jni::String (jni::jint, jni::String)>(env, "onURL");

    return jni::Make<std::string>(env,
        callback.Call(env, method, kind,
            jni::Make<jni::String>(env, url_)));
}

} // namespace android
} // namespace mbgl
