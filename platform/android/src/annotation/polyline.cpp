#include "polyline.hpp"

#include "../conversion/color.hpp"

namespace mbgl {
namespace android {

jni::Class<Polyline> Polyline::javaClass;

mbgl::LineAnnotation Polyline::toAnnotation(jni::JNIEnv& env, jni::Object<Polyline> polyline) {
    auto points = Polyline::getPoints(env, polyline);
    auto holePoints = Polyline::getHolePoints(env, polyline);
    mbgl::LineAnnotation annotation { MultiPoint::toGeometry<mbgl::LineString<double>>(env, points,
                                                                                       holePoints) };
    annotation.opacity = { Polyline::getOpacity(env, polyline) };
    annotation.color = { Polyline::getColor(env, polyline) };
    annotation.width = { Polyline::getWidth(env, polyline) };

    jni::DeleteLocalRef(env, points);
    jni::DeleteLocalRef(env, holePoints);

    return annotation;
}

jni::Object<java::util::List> Polyline::getPoints(jni::JNIEnv& env, jni::Object<Polyline> polyline) {
    static auto field = Polyline::javaClass.GetField<jni::Object<java::util::List>>(env, "points");
    return polyline.Get(env, field);
}

jni::Object<java::util::List> Polyline::getHolePoints(jni::JNIEnv& env, jni::Object<Polyline> polyline) {
    static auto field = Polyline::javaClass.GetField<jni::Object<java::util::List>>(env, "holePoints");
    return polyline.Get(env, field);
}

float Polyline::getOpacity(jni::JNIEnv& env, jni::Object<Polyline> polyline) {
    static auto field = Polyline::javaClass.GetField<float>(env, "alpha");
    return polyline.Get(env, field);
}

mbgl::Color Polyline::getColor(jni::JNIEnv& env, jni::Object<Polyline> polyline) {
    static auto field = Polyline::javaClass.GetField<int>(env, "color");
    return *conversion::convert<mbgl::Color, int>(env, polyline.Get(env, field));
}

float Polyline::getWidth(jni::JNIEnv& env, jni::Object<Polyline> polyline) {
    static auto field = Polyline::javaClass.GetField<float>(env, "width");
    return polyline.Get(env, field);
}

void Polyline::registerNative(jni::JNIEnv& env) {
    Polyline::javaClass = *jni::Class<Polyline>::Find(env).NewGlobalRef(env).release();
}


} // namespace android
} // namespace mbgl