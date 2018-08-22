#include "bitmap_factory.hpp"

namespace mbgl {
namespace android {

void BitmapFactory::registerNative(jni::JNIEnv& env) {
    jni::Class<BitmapFactory>::Singleton(env);
}

jni::Local<jni::Object<Bitmap>> BitmapFactory::DecodeByteArray(jni::JNIEnv& env,
                                                               jni::Array<jni::jbyte>& data,
                                                               jni::jint offset,
                                                               jni::jint length) {
    static auto& _class = jni::Class<BitmapFactory>::Singleton(env);

    // Images are loaded with ARGB_8888 config, and premultiplied by default, which is exactly
    // what we want, so we're not providing a BitmapFactory.Options object.
    using Signature = jni::Object<Bitmap> (jni::Array<jni::jbyte>, jni::jint, jni::jint);
    static auto method = _class.GetStaticMethod<Signature>(env, "decodeByteArray");

    return _class.Call(env, method, data, offset, length);
}

} // namespace android
} // namespace mbgl
