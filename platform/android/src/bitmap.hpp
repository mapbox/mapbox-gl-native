#pragma once

#include <mbgl/util/image.hpp>

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class Bitmap {
public:
    class Config {
    public:
        static constexpr auto Name() {
            return "android/graphics/Bitmap$Config";
        };
        static void registerNative(jni::JNIEnv&);

        enum Value {
            ALPHA_8,
            ARGB_4444,
            ARGB_8888,
            RGB_565,
        };

        static jni::Object<Config> Create(jni::JNIEnv&, Value);

    private:
        static jni::Class<Config> _class;
    };

    static constexpr auto Name() {
        return "android/graphics/Bitmap";
    };
    static void registerNative(jni::JNIEnv&);

    static jni::Object<Bitmap>
    CreateBitmap(jni::JNIEnv&, jni::jint width, jni::jint height, jni::Object<Config>);
    static jni::Object<Bitmap>
    CreateBitmap(jni::JNIEnv& env, jni::jint width, jni::jint height, Config::Value config) {
        return CreateBitmap(env, width, height, Config::Create(env, config));
    }

    static PremultipliedImage GetImage(jni::JNIEnv&, jni::Object<Bitmap>);
    static jni::Object<Bitmap> CreateBitmap(jni::JNIEnv&, const PremultipliedImage&);

private:
    static jni::Class<Bitmap> _class;
};

} // namespace android
} // namespace mbgl
