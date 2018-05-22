#include "bitmap.hpp"

#include <android/bitmap.h>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace android {

class PixelGuard {
public:
    PixelGuard(jni::JNIEnv& env_, jni::Object<Bitmap> bitmap_) : env(env_), bitmap(bitmap_) {
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

    const auto* get() const {
        return address;
    }

private:
    jni::JNIEnv& env;
    jni::Object<Bitmap> bitmap;
    uint8_t* address;
};

void Bitmap::Config::registerNative(jni::JNIEnv& env) {
    _class = *jni::Class<Config>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Bitmap::Config> Bitmap::Config::_class;

jni::Object<Bitmap::Config> Bitmap::Config::Create(jni::JNIEnv& env, Value value) {
    switch (value) {
    case ALPHA_8:
        return _class.Get(env,
                          jni::StaticField<Config, jni::Object<Config>>(env, _class, "ALPHA_8"));
    case ARGB_4444:
        return _class.Get(env,
                          jni::StaticField<Config, jni::Object<Config>>(env, _class, "ARGB_4444"));
    case ARGB_8888:
        return _class.Get(env,
                          jni::StaticField<Config, jni::Object<Config>>(env, _class, "ARGB_8888"));
    case RGB_565:
        return _class.Get(env,
                          jni::StaticField<Config, jni::Object<Config>>(env, _class, "RGB_565"));
    default:
        throw std::runtime_error("invalid enum value for Bitmap.Config");
    }
}

void Bitmap::registerNative(jni::JNIEnv& env) {
    _class = *jni::Class<Bitmap>::Find(env).NewGlobalRef(env).release();
    Config::registerNative(env);
}

jni::Class<Bitmap> Bitmap::_class;

jni::Object<Bitmap> Bitmap::CreateBitmap(jni::JNIEnv& env,
                                         jni::jint width,
                                         jni::jint height,
                                         jni::Object<Config> config) {
    using Signature = jni::Object<Bitmap>(jni::jint, jni::jint, jni::Object<Config>);
    auto method = _class.GetStaticMethod<Signature>(env, "createBitmap");
    return _class.Call(env, method, width, height, config);
}

jni::Object<Bitmap> Bitmap::CreateBitmap(jni::JNIEnv& env, const PremultipliedImage& image) {
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

PremultipliedImage Bitmap::GetImage(jni::JNIEnv& env, jni::Object<Bitmap> bitmap) {
    AndroidBitmapInfo info;
    const int result = AndroidBitmap_getInfo(&env, jni::Unwrap(*bitmap), &info);
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        // TODO: more specific information
        throw std::runtime_error("bitmap decoding: couldn't get bitmap info");
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        bitmap = Bitmap::Copy(env, bitmap);
    }

    const PixelGuard guard(env, bitmap);

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

jni::Object<Bitmap> Bitmap::Copy(jni::JNIEnv& env, jni::Object<Bitmap> bitmap) {
    using Signature = jni::Object<Bitmap>(jni::Object<Config>, jni::jboolean);
    auto static method = _class.GetMethod<Signature>(env, "copy");
    auto config = Bitmap::Config::Create(env, Bitmap::Config::Value::ARGB_8888);
    return bitmap.Call(env, method, config, (jni::jboolean) false);
}

} // namespace android
} // namespace mbgl
