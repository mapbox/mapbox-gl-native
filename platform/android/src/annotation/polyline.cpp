#include "polyline.hpp"

#include "../conversion/color.hpp"

namespace mbgl {
namespace android {

mbgl::LineAnnotation Polyline::toAnnotation(jni::JNIEnv& env, const jni::Object<Polyline>& polyline) {
    static auto& javaClass = jni::Class<Polyline>::Singleton(env);
    static auto points = javaClass.GetField<jni::Object<java::util::List>>(env, "points");
    static auto alpha = javaClass.GetField<float>(env, "alpha");
    static auto color = javaClass.GetField<int>(env, "color");
    static auto width = javaClass.GetField<float>(env, "width");

    mbgl::LineAnnotation annotation {
        MultiPoint::toGeometry<mbgl::LineString<double>>(env, polyline.Get(env, points))
    };

    annotation.opacity = polyline.Get(env, alpha);
    annotation.color = *conversion::convert<mbgl::Color>(env, polyline.Get(env, color));
    annotation.width = polyline.Get(env, width);

    return annotation;
}

void Polyline::registerNative(jni::JNIEnv& env) {
    jni::Class<Polyline>::Singleton(env);
}

} // namespace android
} // namespace mbgl