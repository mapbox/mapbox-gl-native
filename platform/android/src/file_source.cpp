#include "file_source.hpp"

#include <mbgl/util/logging.hpp>

#include <string>


namespace mbgl {
namespace android {

FileSource::FileSource(jni::JNIEnv& _env, jni::String _cachePath, jni::String _apkPath) {
    Log::Info(Event::JNI, "FileSource::FileSource");

    // Create a core default file source
    fileSource = std::make_unique<mbgl::DefaultFileSource>(
        jni::Make<std::string>(_env, _cachePath) + "/mbgl-offline.db",
        jni::Make<std::string>(_env, _apkPath));
}

FileSource::~FileSource() {
    Log::Info(Event::JNI, "FileSource::~FileSource");
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
    FileSource::javaClass = *jni::Class<FileSource>::Find(env).NewGlobalRef(env).release();

    #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<FileSource>(
        env, FileSource::javaClass, "nativePtr",
        std::make_unique<FileSource, JNIEnv&, jni::String, jni::String>,
        "initialize",
        "finalize"
    );
}


} // namespace android
} // namespace mbgl