#include "bitmap.hpp"

#include <android/bitmap.h>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace android {

class PixelGuard {
public:
    PixelGuard(jni::JNIEnv& env_, const jni::Object<Bitmap>& bitmap_) : env(env_), bitmap(bitmap_) {
        const int result = AndroidBitmap_lockPixels(&env, jni::Unwrap(*bitmap),
                                                    reinterpret_cast<void**>(&address));
        if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
            throw std::runtime_error("bitmap decoding: could not lock pixels");
        }
    }

    ~PixelGuard() {
        const int result = AndroidBitmap_unlockPixels(&env, jni::Unwrap(*bitmap));
        if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
            Log::Warning(mbgl::Event::General, "Bitmap decoding: could not unlock pixels");
        }
    }

    auto* get() {
        return address;
    }

private:
    jni::JNIEnv& env;
    const jni::Object<Bitmap>& bitmap;
    uint8_t* address;
};

jni::Local<jni::Object<Bitmap::Config>> Bitmap::Config::Create(jni::JNIEnv& env, Value value) {
    static auto& _class = jni::Class<Config>::Singleton(env);
    switch (value) {
    case ALPHA_8:
        return _class.Get(env, _class.GetStaticField<jni::Object<Config>>(env, "ALPHA_8"));
    case ARGB_4444:
        return _class.Get(env, _class.GetStaticField<jni::Object<Config>>(env, "ARGB_4444"));
    case ARGB_8888:
        return _class.Get(env, _class.GetStaticField<jni::Object<Config>>(env, "ARGB_8888"));
    case RGB_565:
        return _class.Get(env, _class.GetStaticField<jni::Object<Config>>(env, "RGB_565"));
    default:
        throw std::runtime_error("invalid enum value for Bitmap.Config");
    }
}

void Bitmap::registerNative(jni::JNIEnv& env) {
    jni::Class<Bitmap>::Singleton(env);
    jni::Class<Bitmap::Config>::Singleton(env);
}

jni::Local<jni::Object<Bitmap>> Bitmap::CreateBitmap(jni::JNIEnv& env,
                                                     jni::jint width,
                                                     jni::jint height,
                                                     const jni::Object<Config>& config) {
    static auto& _class = jni::Class<Bitmap>::Singleton(env);
    static auto method = _class.GetStaticMethod<jni::Object<Bitmap> (jni::jint, jni::jint, jni::Object<Config>)>(env, "createBitmap");

    return _class.Call(env, method, width, height, config);
}

PremultipliedImage Bitmap::GetImage(jni::JNIEnv& env, const jni::Object<Bitmap>& bitmap) {
    AndroidBitmapInfo info;
    const int result = AndroidBitmap_getInfo(&env, jni::Unwrap(*bitmap), &info);
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        throw std::runtime_error("bitmap decoding: couldn't get bitmap info");
    }
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        return Bitmap::GetImage(env, Bitmap::Copy(env, bitmap));
    }

    PixelGuard guard(env, bitmap);

    // Copy the Android Bitmap into the PremultipliedImage.
    auto pixels =
        std::make_unique<uint8_t[]>(info.width * info.height * PremultipliedImage::channels);
    for (uint32_t y = 0; y < info.height; y++) {
        auto begin = guard.get() + y * info.stride;
        std::copy(begin, begin + info.width * PremultipliedImage::channels,
            pixels.get() + y * info.width * PremultipliedImage::channels);
    }

    return { Size{ info.width, info.height }, std::move(pixels) };
}

jni::Local<jni::Object<Bitmap>> Bitmap::CreateBitmap(jni::JNIEnv& env, const PremultipliedImage& image) {
    auto bitmap = CreateBitmap(env, image.size.width, image.size.height, Config::ARGB_8888);

    AndroidBitmapInfo info;
    const int result = AndroidBitmap_getInfo(&env, jni::Unwrap(*bitmap), &info);
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        // TODO: more specific information
        throw std::runtime_error("bitmap creation: couldn't get bitmap info");
    }

    assert(info.width == image.size.width);
    assert(info.height == image.size.height);
    assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888);

    PixelGuard guard(env, bitmap);

    // Copy the PremultipliedImage into the Android Bitmap
    for (uint32_t y = 0; y < image.size.height; y++) {
        auto begin = image.data.get() + y * image.stride();
        std::copy(begin, begin + image.stride(), guard.get() + y * info.stride);
    }

    return bitmap;
}

jni::Local<jni::Object<Bitmap>> Bitmap::Copy(jni::JNIEnv& env, const jni::Object<Bitmap>& bitmap) {
    static auto& klass = jni::Class<Bitmap>::Singleton(env);
    static auto copy = klass.GetMethod<jni::Object<Bitmap> (jni::Object<Config>, jni::jboolean)>(env, "copy");

    return bitmap.Call(env, copy, Bitmap::Config::Create(env, Bitmap::Config::Value::ARGB_8888), jni::jni_false);
}

} // namespace android
} // namespace mbgl
