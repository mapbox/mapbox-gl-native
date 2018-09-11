#include "projected_meters.hpp"

namespace mbgl {
namespace android {

jni::Local<jni::Object<ProjectedMeters>> ProjectedMeters::New(jni::JNIEnv& env, double northing, double easting) {
    static auto& javaClass = jni::Class<ProjectedMeters>::Singleton(env);
    static auto constructor = javaClass.GetConstructor<double, double>(env);
    return javaClass.New(env, constructor, northing, easting);
}

void ProjectedMeters::registerNative(jni::JNIEnv& env) {
    jni::Class<ProjectedMeters>::Singleton(env);
}

} // namespace android
} // namespace mbgl