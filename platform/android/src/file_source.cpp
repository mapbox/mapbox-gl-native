#include "file_source.hpp"
#include "attach_env.hpp"

#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/storage/resource_transform.hpp>
#include <mbgl/util/logging.hpp>

#include <sqlite3.hpp>

#include "asset_manager_file_source.hpp"

namespace mbgl {
namespace android {

// FileSource //

FileSource::FileSource(jni::JNIEnv& _env,
                       const jni::String& accessToken,
                       const jni::String& _cachePath,
                       const jni::Object<AssetManager>& assetManager) {
    mapbox::sqlite::setTempPath(jni::Make<std::string>(_env, _cachePath));

    // Create a core default file source
    fileSource = std::make_unique<mbgl::DefaultFileSource>(
        jni::Make<std::string>(_env, _cachePath) + "/mbgl-offline.db",
        std::make_unique<AssetManagerFileSource>(_env, assetManager));

    // Set access token
    if (accessToken) {
        fileSource->setAccessToken(jni::Make<std::string>(_env, accessToken));
    }
}

FileSource::~FileSource() {
}

jni::Local<jni::String> FileSource::getAccessToken(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, fileSource->getAccessToken());
}

void FileSource::setAccessToken(jni::JNIEnv& env, const jni::String& token) {
    fileSource->setAccessToken(jni::Make<std::string>(env, token));
}

void FileSource::setAPIBaseUrl(jni::JNIEnv& env, const jni::String& url) {
    fileSource->setAPIBaseURL(jni::Make<std::string>(env, url));
}

void FileSource::setResourceTransform(jni::JNIEnv& env, const jni::Object<FileSource::ResourceTransformCallback>& transformCallback) {
    if (transformCallback) {
        auto global = jni::NewGlobal<jni::EnvAttachingDeleter>(env, transformCallback);
        resourceTransform = std::make_unique<Actor<ResourceTransform>>(*Scheduler::GetCurrent(),
            // Capture the ResourceTransformCallback object as a managed global into
            // the lambda. It is released automatically when we're setting a new ResourceTransform in
            // a subsequent call.
            // Note: we're converting it to shared_ptr because this lambda is converted to a std::function,
            // which requires copyability of its captured variables.
            [callback = std::make_shared<decltype(global)>(std::move(global))]
            (mbgl::Resource::Kind kind, const std::string&& url_) {
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

mbgl::DefaultFileSource& FileSource::getDefaultFileSource(jni::JNIEnv& env, const jni::Object<FileSource>& jFileSource) {
    FileSource* fileSource = FileSource::getNativePeer(env, jFileSource);
    assert(fileSource != nullptr);
    return *fileSource->fileSource;
}

void FileSource::registerNative(jni::JNIEnv& env) {
    // Ensure the class for ResourceTransformCallback is cached. If it's requested for the
    // first time on a background thread, Android's class loader heuristics will fail.
    // https://developer.android.com/training/articles/perf-jni#faq_FindClass
    jni::Class<ResourceTransformCallback>::Singleton(env);

    static auto& javaClass = jni::Class<FileSource>::Singleton(env);

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<FileSource>(
        env, javaClass, "nativePtr",
        jni::MakePeer<FileSource, const jni::String&, const jni::String&, const jni::Object<AssetManager>&>,
        "initialize",
        "finalize",
        METHOD(&FileSource::getAccessToken, "getAccessToken"),
        METHOD(&FileSource::setAccessToken, "setAccessToken"),
        METHOD(&FileSource::setAPIBaseUrl, "setApiBaseUrl"),
        METHOD(&FileSource::setResourceTransform, "setResourceTransform"),
        METHOD(&FileSource::resume, "activate"),
        METHOD(&FileSource::pause, "deactivate"),
        METHOD(&FileSource::isResumed, "isActivated")
    );
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
