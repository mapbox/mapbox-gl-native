#include "polygon.hpp"

#include "../conversion/color.hpp"

namespace mbgl {
namespace android {

jni::Class<Polygon> Polygon::javaClass;

mbgl::FillAnnotation Polygon::toAnnotation(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    auto points = Polygon::getPoints(env, polygon);
    auto holes = Polygon::getHoles(env, polygon);

    mbgl::Polygon<double> geometry { MultiPoint::toGeometry<mbgl::LinearRing<double>>(env, points) };

    auto jHoleListsArray = java::util::List::toArray<java::util::List>(env, holes);
    std::size_t jHoleListsSize = jHoleListsArray.Length(env);
    for (std::size_t i = 0; i < jHoleListsSize; i++) {
        auto jHoleList = jHoleListsArray.Get(env, i);
        geometry.push_back(MultiPoint::toGeometry<mbgl::LinearRing<double>>(env, jHoleList));
        jni::DeleteLocalRef(env, jHoleList);
    }
    jni::DeleteLocalRef(env, jHoleListsArray);

    mbgl::FillAnnotation annotation { geometry };
    annotation.opacity = { Polygon::getOpacity(env, polygon) };
    annotation.color = { Polygon::getFillColor(env, polygon) };
    annotation.outlineColor = { Polygon::getOutlineColor(env, polygon) };

    jni::DeleteLocalRef(env, points);
    jni::DeleteLocalRef(env, holes);

    return annotation;
}

jni::Object<java::util::List> Polygon::getPoints(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    static auto field = Polygon::javaClass.GetField<jni::Object<java::util::List>>(env, "points");
    return polygon.Get(env, field);
}

jni::Object<java::util::List> Polygon::getHoles(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    static auto field = Polygon::javaClass.GetField<jni::Object<java::util::List>>(env, "holes");
    return polygon.Get(env, field);
}

float Polygon::getOpacity(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    static auto field = Polygon::javaClass.GetField<float>(env, "alpha");
    return polygon.Get(env, field);
}

mbgl::Color Polygon::getFillColor(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    static auto field = Polygon::javaClass.GetField<int>(env, "fillColor");
    return *conversion::convert<mbgl::Color, int>(env, polygon.Get(env, field));
}

mbgl::Color Polygon::getOutlineColor(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    static auto field = Polygon::javaClass.GetField<int>(env, "strokeColor");
    return *conversion::convert<mbgl::Color, int>(env, polygon.Get(env, field));
}

void Polygon::registerNative(jni::JNIEnv& env) {
    Polygon::javaClass = *jni::Class<Polygon>::Find(env).NewGlobalRef(env).release();
}


} // namespace android
} // namespace mbgl