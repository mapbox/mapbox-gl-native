#include <mbgl/util/geo.hpp>
#include "pointf.hpp"

namespace mbgl {
namespace android {

jni::Object<PointF> PointF::New(jni::JNIEnv& env, float x, float y) {
    static auto constructor = PointF::javaClass.GetConstructor<float, float>(env);
    return PointF::javaClass.New(env, constructor, x, y);
}

mbgl::ScreenCoordinate PointF::getScreenCoordinate(jni::JNIEnv& env, jni::Object<PointF> point) {
    static auto xField = PointF::javaClass.GetField<jni::jfloat>(env, "x");
    static auto yField = PointF::javaClass.GetField<jni::jfloat>(env, "y");
    return mbgl::ScreenCoordinate{point.Get(env, xField), point.Get(env, yField)};
}

void PointF::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    PointF::javaClass = *jni::Class<PointF>::Find(env).NewGlobalRef(env).release();
}

jni::Class<PointF> PointF::javaClass;


} // namespace android
} // namespace mbgl