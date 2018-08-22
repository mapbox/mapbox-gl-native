#include <mbgl/style/image.hpp>
#include <mbgl/util/exception.hpp>
#include "image.hpp"

namespace mbgl {
namespace android {

mbgl::style::Image Image::getImage(jni::JNIEnv& env, const jni::Object<Image>& image) {
    static auto& javaClass = jni::Class<Image>::Singleton(env);
    static auto widthField = javaClass.GetField<jni::jint>(env, "width");
    static auto heightField = javaClass.GetField<jni::jint>(env, "height");
    static auto pixelRatioField = javaClass.GetField<jni::jfloat>(env, "pixelRatio");
    static auto bufferField = javaClass.GetField<jni::Array<jbyte>>(env, "buffer");
    static auto nameField = javaClass.GetField<jni::String>(env, "name");
    static auto sdfField = javaClass.GetField<jni::jboolean>(env, "sdf");

    auto height = image.Get(env, heightField);
    auto width = image.Get(env, widthField);
    auto pixelRatio = image.Get(env, pixelRatioField);
    auto pixels = image.Get(env, bufferField);
    auto name = jni::Make<std::string>(env, image.Get(env, nameField));
    auto sdf = (bool) image.Get(env, sdfField);

    jni::NullCheck(env, pixels.get());
    std::size_t size = pixels.Length(env);

    mbgl::PremultipliedImage premultipliedImage({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
    if (premultipliedImage.bytes() != uint32_t(size)) {
        throw mbgl::util::SpriteImageException("Sprite image pixel count mismatch");
    }

    jni::GetArrayRegion(env, *pixels, 0, size, reinterpret_cast<jbyte*>(premultipliedImage.data.get()));
    return mbgl::style::Image {name, std::move(premultipliedImage), pixelRatio, sdf};
}

void Image::registerNative(jni::JNIEnv &env) {
    jni::Class<Image>::Singleton(env);
}

} // namespace android
} // namespace mb
