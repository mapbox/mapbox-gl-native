#pragma once

#include <mbgl/storage/default_file_source.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

/**
 * Peer class for the Android FileSource holder. Ensures that a single DefaultFileSource is used
 */
class FileSource {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/fs/FileSource"; };

    FileSource(jni::JNIEnv&, jni::String, jni::String);

    ~FileSource();

    static jni::Class<FileSource> javaClass;

    static FileSource* getNativePeer(jni::JNIEnv&, jni::Object<FileSource>);

    static mbgl::DefaultFileSource& getDefaultFileSource(jni::JNIEnv&, jni::Object<FileSource>);

    static void registerNative(jni::JNIEnv&);

private:

    std::unique_ptr<mbgl::DefaultFileSource> fileSource;
};


} // namespace android
} // namespace mbgl