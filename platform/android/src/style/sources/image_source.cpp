#include "image_source.hpp"

// Java -> C++ conversion
#include "../android_conversion.hpp"

// C++ -> Java conversion
#include "../../conversion/conversion.hpp"
#include <mbgl/style/conversion.hpp>
#include <mbgl/util/premultiply.hpp>

#include "../../bitmap.hpp"
#include <string>
#include <array>

namespace mbgl {
namespace android {

    ImageSource::ImageSource(jni::JNIEnv& env, jni::String sourceId, jni::Object<LatLngQuad> coordinatesObject)
        : Source(env, std::make_unique<mbgl::style::ImageSource>(
                jni::Make<std::string>(env, sourceId),
                LatLngQuad::getLatLngArray(env, coordinatesObject)
                )
            ) {
    }

    ImageSource::ImageSource(mbgl::Map& map, mbgl::style::ImageSource& coreSource)
        : Source(map, coreSource) {
    }

    ImageSource::~ImageSource() = default;

    void ImageSource::setURL(jni::JNIEnv& env, jni::String url) {
        // Update the core source
        source.as<mbgl::style::ImageSource>()->ImageSource::setURL(jni::Make<std::string>(env, url));
    }

    jni::String ImageSource::getURL(jni::JNIEnv& env) {
        optional<std::string> url = source.as<mbgl::style::ImageSource>()->ImageSource::getURL();
        return url ? jni::Make<jni::String>(env, *url) : jni::String();
    }

    void ImageSource::setImage(jni::JNIEnv& env, jni::Object<Bitmap> bitmap) {
        UnassociatedImage image = util::unpremultiply(Bitmap::GetImage(env, bitmap));
        source.as<mbgl::style::ImageSource>()->setImage(std:: move(image));
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
                std::make_unique<ImageSource, JNIEnv&, jni::String, jni::Object<LatLngQuad>>,
                "initialize",
                "finalize",
                METHOD(&ImageSource::setURL, "nativeSetUrl"),
                METHOD(&ImageSource::getURL, "nativeGetUrl"),
                METHOD(&ImageSource::setImage, "nativeSetImage")
        );
    }

} // namespace android
} // namespace mbgl
