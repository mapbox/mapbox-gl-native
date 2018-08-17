#include "polyline.hpp"

#include "../conversion/color.hpp"

namespace mbgl {
namespace android {

mbgl::LineAnnotation Polyline::toAnnotation(jni::JNIEnv& env, jni::Object<Polyline> polyline) {
    auto points = jni::SeizeLocal(env, Polyline::getPoints(env, polyline));

    mbgl::LineAnnotation annotation { MultiPoint::toGeometry<mbgl::LineString<double>>(env, *points) };
    annotation.opacity = { Polyline::getOpacity(env, polyline) };
    annotation.color = { Polyline::getColor(env, polyline) };
    annotation.width = { Polyline::getWidth(env, polyline) };

    return annotation;
}

jni::Object<java::util::List> Polyline::getPoints(jni::JNIEnv& env, jni::Object<Polyline> polyline) {
    static auto javaClass = jni::Class<Polyline>::Singleton(env);
    static auto field = javaClass.GetField<jni::Object<java::util::List>>(env, "points");
    return polyline.Get(env, field);
}

float Polyline::getOpacity(jni::JNIEnv& env, jni::Object<Polyline> polyline) {
    static auto javaClass = jni::Class<Polyline>::Singleton(env);
    static auto field = javaClass.GetField<float>(env, "alpha");
    return polyline.Get(env, field);
}

mbgl::Color Polyline::getColor(jni::JNIEnv& env, jni::Object<Polyline> polyline) {
    static auto javaClass = jni::Class<Polyline>::Singleton(env);
    static auto field = javaClass.GetField<int>(env, "color");
    return *conversion::convert<mbgl::Color, int>(env, polyline.Get(env, field));
}

float Polyline::getWidth(jni::JNIEnv& env, jni::Object<Polyline> polyline) {
    static auto javaClass = jni::Class<Polyline>::Singleton(env);
    static auto field = javaClass.GetField<float>(env, "width");
    return polyline.Get(env, field);
}

void Polyline::registerNative(jni::JNIEnv& env) {
    jni::Class<Polyline>::Singleton(env);
}

} // namespace android
} // namespace mbgl