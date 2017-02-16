#include "projected_meters.hpp"

namespace mbgl {
namespace android {

jni::Object<ProjectedMeters> ProjectedMeters::New(jni::JNIEnv& env, double northing, double easting) {
    static auto constructor = ProjectedMeters::javaClass.GetConstructor<double, double>(env);
    return ProjectedMeters::javaClass.New(env, constructor, northing, easting);
}

void ProjectedMeters::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    ProjectedMeters::javaClass = *jni::Class<ProjectedMeters>::Find(env).NewGlobalRef(env).release();
}

jni::Class<ProjectedMeters> ProjectedMeters::javaClass;


} // namespace android
} // namespace mbgl