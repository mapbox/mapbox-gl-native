#include <mbgl/style/image.hpp>
#include <mbgl/util/exception.hpp>
#include "image.hpp"

namespace mbgl {
namespace android {

mbgl::style::Image Image::getImage(jni::JNIEnv& env, jni::Object<Image> image) {
    static auto widthField = Image::javaClass.GetField<jni::jint>(env, "width");
    static auto heightField = Image::javaClass.GetField<jni::jint>(env, "height");
    static auto pixelRatioField = Image::javaClass.GetField<jni::jfloat>(env, "pixelRatio");
    static auto bufferField = Image::javaClass.GetField<jni::Array<jbyte>>(env, "buffer");
    static auto nameField = Image::javaClass.GetField<jni::String>(env, "name");

    auto height = image.Get(env, heightField);
    auto width = image.Get(env, widthField);
    auto pixelRatio = image.Get(env, pixelRatioField);
    auto pixels = image.Get(env, bufferField);
    auto jName = image.Get(env, nameField);
    auto name = jni::Make<std::string>(env, jName);
    jni::DeleteLocalRef(env, jName);

    jni::NullCheck(env, &pixels);
    std::size_t size = pixels.Length(env);

    mbgl::PremultipliedImage premultipliedImage({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
    if (premultipliedImage.bytes() != uint32_t(size)) {
        throw mbgl::util::SpriteImageException("Sprite image pixel count mismatch");
    }

    jni::GetArrayRegion(env, *pixels, 0, size, reinterpret_cast<jbyte*>(premultipliedImage.data.get()));

    return mbgl::style::Image {name, std::move(premultipliedImage), pixelRatio};
}

void Image::registerNative(jni::JNIEnv &env) {
    // Lookup the class
    Image::javaClass = *jni::Class<Image>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Image> Image::javaClass;


} // namespace android
} // namespace mb

