#include "snapshot.hpp"

#include "../bitmap.hpp"

namespace mbgl {
namespace android {

jni::Object<Snapshot> Snapshot::New(jni::JNIEnv& env, jni::Object<Bitmap> image) {
    static auto constructor = Snapshot::javaClass.GetConstructor<jni::Object<Bitmap>>(env);
    return Snapshot::javaClass.New(env, constructor, image);
}

void Snapshot::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    Snapshot::javaClass = *jni::Class<Snapshot>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Snapshot> Snapshot::javaClass;


} // namespace android
} // namespace mbgl