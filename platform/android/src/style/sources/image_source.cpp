#include "image_source.hpp"

// Java -> C++ conversion
#include "../android_conversion.hpp"

// C++ -> Java conversion
#include "../../conversion/conversion.hpp"
#include <mbgl/style/conversion.hpp>

#include <string>

namespace mbgl {
namespace android {

    ImageSource::ImageSource(jni::JNIEnv& env, jni::String sourceId)
        : Source(env, std::make_unique<mbgl::style::ImageSource>(
                jni::Make<std::string>(env, sourceId)
                )
            ) {
    }

    ImageSource::ImageSource(mbgl::Map& map, mbgl::style::ImageSource& coreSource)
        : Source(map, coreSource) {
    }

    ImageSource::~ImageSource() = default;

    void GeoJSONSource::setGeoJSONString(jni::JNIEnv& env, jni::String json) {
        using namespace mbgl::style::conversion;

        // Convert the jni object
        Error error;
        optional<GeoJSON> converted = convert<GeoJSON>(Value(env, json), error);
        if(!converted) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting geo json: " + error.message);
            return;
        }

        // Update the core source
        source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::setGeoJSON(*converted);
    }

    void ImageSource::setURL(jni::JNIEnv& env, jni::String url) {
        // Update the core source
        source.as<mbgl::style::GeoJSONSource>()->GeoJSONSource::setURL(jni::Make<std::string>(env, url));
    }

    jni::String ImageSource::getURL(jni::JNIEnv& env) {
        std::string url = source.as<mbgl::style::ImageSource>()->ImageSource::getURL();
        return !url.empty() ? jni::Make<jni::String>(env, url) : jni::String();
    }


    jni::Class<ImageSource> ImageSource::javaClass;

    jni::jobject* ImageSource::createJavaPeer(jni::JNIEnv& env) {
        static auto constructor = ImageSource::javaClass.template GetConstructor<jni::jlong>(env);
        return ImageSource::javaClass.New(env, constructor, reinterpret_cast<jni::jlong>(this));
    }

    void ImageSource::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        ImageSource::javaClass = *jni::Class<ImageSource>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<ImageSource>(
            env, ImageSource::javaClass, "nativePtr",
            std::make_unique<ImageSource, JNIEnv&, jni::String, jni::Object<>>,
            "initialize",
            "finalize",
            METHOD(&ImageSource::setGeoJSONString, "nativeSetGeoJsonString"),
            METHOD(&ImageSource::setURL, "nativeSetUrl"),
            METHOD(&ImageSource::getURL, "nativeGetUrl"),
            METHOD(&ImageSource::getURL, "nativeGetUrl"),
        );
    }

} // namespace android
} // namespace mbgl
