#include "lat_lng_quad.hpp"
#include "lat_lng.hpp"

namespace mbgl {
namespace android {

jni::Local<jni::Object<LatLngQuad>> LatLngQuad::New(jni::JNIEnv& env, std::array<mbgl::LatLng, 4> coordinates) {
    static auto& javaClass = jni::Class<LatLngQuad>::Singleton(env);
    static auto quadConstructor = javaClass.GetConstructor<jni::Object<LatLng>, jni::Object<LatLng>, jni::Object<LatLng>, jni::Object<LatLng>>(env);
    return javaClass.New(env, quadConstructor,
        LatLng::New(env, coordinates[0]),
        LatLng::New(env, coordinates[1]),
        LatLng::New(env, coordinates[2]),
        LatLng::New(env, coordinates[3]));
}

std::array<mbgl::LatLng, 4> LatLngQuad::getLatLngArray(jni::JNIEnv& env, const jni::Object<LatLngQuad>& quad) {
    static auto& javaClass = jni::Class<LatLngQuad>::Singleton(env);
    static auto topLeftField = javaClass.GetField <jni::Object<LatLng>>(env, "topLeft");
    static auto topRightField = javaClass.GetField <jni::Object<LatLng>>(env, "topRight");
    static auto bottomRightField = javaClass.GetField <jni::Object<LatLng>>(env, "bottomRight");
    static auto bottomLeftField = javaClass.GetField <jni::Object<LatLng>>(env, "bottomLeft");

    return std::array < mbgl::LatLng, 4 > {{
        LatLng::getLatLng(env, quad.Get(env, topLeftField)),
        LatLng::getLatLng(env, quad.Get(env, topRightField)),
        LatLng::getLatLng(env, quad.Get(env, bottomRightField)),
        LatLng::getLatLng(env, quad.Get(env, bottomLeftField))
    }};
}

void LatLngQuad::registerNative(jni::JNIEnv& env) {
    jni::Class<LatLngQuad>::Singleton(env);
}

} // namespace android
} // namespace mbgl