#include "file_source.hpp"

#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/storage/resource_transform.hpp>
#include <mbgl/util/logging.hpp>

#include "asset_manager_file_source.hpp"
#include "jni/generic_global_ref_deleter.hpp"

namespace mbgl {
namespace android {

// FileSource //

FileSource::FileSource(jni::JNIEnv& _env,
                       jni::String accessToken,
                       jni::String _cachePath,
                       jni::Object<AssetManager> assetManager) {
    // Create a core default file source
    fileSource = std::make_unique<mbgl::DefaultFileSource>(
        jni::Make<std::string>(_env, _cachePath) + "/mbgl-offline.db",
        std::make_unique<AssetManagerFileSource>(_env, assetManager));

    // Set access token
    fileSource->setAccessToken(jni::Make<std::string>(_env, accessToken));
}

FileSource::~FileSource() {
}

jni::String FileSource::getAccessToken(jni::JNIEnv& env) {
    return jni::Make<jni::String>(env, fileSource->getAccessToken());
}

void FileSource::setAccessToken(jni::JNIEnv& env, jni::String token) {
    fileSource->setAccessToken(jni::Make<std::string>(env, token));
}

void FileSource::setAPIBaseUrl(jni::JNIEnv& env, jni::String url) {
    fileSource->setAPIBaseURL(jni::Make<std::string>(env, url));
}

void FileSource::setResourceTransform(jni::JNIEnv& env, jni::Object<FileSource::ResourceTransformCallback> transformCallback) {
    if (transformCallback) {
        resourceTransform = std::make_unique<Actor<ResourceTransform>>(*Scheduler::GetCurrent(),
            // Capture the ResourceTransformCallback object as a managed global into
            // the lambda. It is released automatically when we're setting a new ResourceTransform in
            // a subsequent call.
            // Note: we're converting it to shared_ptr because this lambda is converted to a std::function,
            // which requires copyability of its captured variables.
            [callback = std::shared_ptr<jni::jobject>(transformCallback.NewGlobalRef(env).release()->Get(), GenericGlobalRefDeleter())]
            (mbgl::Resource::Kind kind, const std::string&& url_) {
                android::UniqueEnv _env = android::AttachEnv();
                return FileSource::ResourceTransformCallback::onURL(*_env, jni::Object<FileSource::ResourceTransformCallback>(*callback), int(kind), url_);
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
    activationCounter.value()--;
    if (activationCounter == 0) {
        fileSource->pause();
    }
}

jni::jboolean FileSource::isResumed(jni::JNIEnv&) {
    if (activationCounter) {
       return  (jboolean) (activationCounter > 0);
    }
    return (jboolean) false;
}

jni::Class<FileSource> FileSource::javaClass;

FileSource* FileSource::getNativePeer(jni::JNIEnv& env, jni::Object<FileSource> jFileSource) {
    static auto field = FileSource::javaClass.GetField<jlong>(env, "nativePtr");
    return reinterpret_cast<FileSource *>(jFileSource.Get(env, field));
}

mbgl::DefaultFileSource& FileSource::getDefaultFileSource(jni::JNIEnv& env, jni::Object<FileSource> jFileSource) {
    FileSource* fileSource = FileSource::getNativePeer(env, jFileSource);
    assert(fileSource != nullptr);
    return *fileSource->fileSource;
}

void FileSource::registerNative(jni::JNIEnv& env) {
    //Register classes
    FileSource::javaClass = *jni::Class<FileSource>::Find(env).NewGlobalRef(env).release();
    FileSource::ResourceTransformCallback::javaClass = *jni::Class<FileSource::ResourceTransformCallback>::Find(env).NewGlobalRef(env).release();

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<FileSource>(
        env, FileSource::javaClass, "nativePtr",
        std::make_unique<FileSource, JNIEnv&, jni::String, jni::String, jni::Object<AssetManager>>,
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

jni::Class<FileSource::ResourceTransformCallback> FileSource::ResourceTransformCallback::javaClass;

std::string FileSource::ResourceTransformCallback::onURL(jni::JNIEnv& env, jni::Object<FileSource::ResourceTransformCallback> callback, int kind, std::string url_) {
    static auto method = FileSource::ResourceTransformCallback::javaClass.GetMethod<jni::String (jni::jint, jni::String)>(env, "onURL");
    auto url = jni::Make<jni::String>(env, url_);

    url = callback.Call(env, method, kind, url);
    auto urlStr = jni::Make<std::string>(env, url);
    jni::DeleteLocalRef(env, url);
    return urlStr;
}

} // namespace android
} // namespace mbgl
