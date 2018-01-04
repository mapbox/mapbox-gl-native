#include "lat_lng_quad.hpp"
#include "lat_lng.hpp"

namespace mbgl {
namespace android {

jni::Object<LatLngQuad> LatLngQuad::New(jni::JNIEnv& env, std::array<mbgl::LatLng, 4> coordinates) {
    static auto quadConstructor = LatLngQuad::javaClass.GetConstructor<jni::Object<LatLng>, jni::Object<LatLng>, jni::Object<LatLng>, jni::Object<LatLng>>(env);
    return LatLngQuad::javaClass.New(env, quadConstructor,
        LatLng::New(env, coordinates[0]),
        LatLng::New(env, coordinates[1]),
        LatLng::New(env, coordinates[2]),
        LatLng::New(env, coordinates[3]));
}

std::array<mbgl::LatLng, 4> LatLngQuad::getLatLngArray(jni::JNIEnv& env, jni::Object<LatLngQuad> quad) {
    static auto topLeftField = LatLngQuad::javaClass.GetField <jni::Object<LatLng>>(env, "topLeft");
    static auto topRightField = LatLngQuad::javaClass.GetField <jni::Object<LatLng>>(env, "topRight");
    static auto bottomRightField = LatLngQuad::javaClass.GetField <jni::Object<LatLng>>(env, "bottomRight");
    static auto bottomLeftField = LatLngQuad::javaClass.GetField <jni::Object<LatLng>>(env, "bottomLeft");

    return std::array < mbgl::LatLng, 4 > {{
        LatLng::getLatLng(env, quad.Get(env, topLeftField)),
        LatLng::getLatLng(env, quad.Get(env, topRightField)),
        LatLng::getLatLng(env, quad.Get(env, bottomRightField)),
        LatLng::getLatLng(env, quad.Get(env, bottomLeftField))
    }};
}

void LatLngQuad::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    LatLngQuad::javaClass = *jni::Class<LatLngQuad>::Find(env).NewGlobalRef(env).release();
}

jni::Class<LatLngQuad> LatLngQuad::javaClass;


} // namespace android
} // namespace mbgl