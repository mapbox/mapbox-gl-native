#include "polygon.hpp"

#include "../conversion/color.hpp"

namespace mbgl {
namespace android {

mbgl::FillAnnotation Polygon::toAnnotation(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    auto points = jni::SeizeLocal(env, Polygon::getPoints(env, polygon));
    auto holes = jni::SeizeLocal(env, Polygon::getHoles(env, polygon));

    mbgl::Polygon<double> geometry { MultiPoint::toGeometry<mbgl::LinearRing<double>>(env, *points) };

    auto jHoleListsArray = jni::SeizeLocal(env, java::util::List::toArray<java::util::List>(env, *holes));
    std::size_t jHoleListsSize = jHoleListsArray->Length(env);
    for (std::size_t i = 0; i < jHoleListsSize; i++) {
        geometry.push_back(MultiPoint::toGeometry<mbgl::LinearRing<double>>(env,
            *jni::SeizeLocal(env, jHoleListsArray->Get(env, i))));
    }

    mbgl::FillAnnotation annotation { geometry };
    annotation.opacity = { Polygon::getOpacity(env, polygon) };
    annotation.color = { Polygon::getFillColor(env, polygon) };
    annotation.outlineColor = { Polygon::getOutlineColor(env, polygon) };

    return annotation;
}

jni::Object<java::util::List> Polygon::getPoints(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    static auto javaClass = jni::Class<Polygon>::Singleton(env);
    static auto field = javaClass.GetField<jni::Object<java::util::List>>(env, "points");
    return polygon.Get(env, field);
}

jni::Object<java::util::List> Polygon::getHoles(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    static auto javaClass = jni::Class<Polygon>::Singleton(env);
    static auto field = javaClass.GetField<jni::Object<java::util::List>>(env, "holes");
    return polygon.Get(env, field);
}

float Polygon::getOpacity(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    static auto javaClass = jni::Class<Polygon>::Singleton(env);
    static auto field = javaClass.GetField<float>(env, "alpha");
    return polygon.Get(env, field);
}

mbgl::Color Polygon::getFillColor(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    static auto javaClass = jni::Class<Polygon>::Singleton(env);
    static auto field = javaClass.GetField<int>(env, "fillColor");
    return *conversion::convert<mbgl::Color, int>(env, polygon.Get(env, field));
}

mbgl::Color Polygon::getOutlineColor(jni::JNIEnv& env, jni::Object<Polygon> polygon) {
    static auto javaClass = jni::Class<Polygon>::Singleton(env);
    static auto field = javaClass.GetField<int>(env, "strokeColor");
    return *conversion::convert<mbgl::Color, int>(env, polygon.Get(env, field));
}

void Polygon::registerNative(jni::JNIEnv& env) {
    jni::Class<Polygon>::Singleton(env);
}

} // namespace android
} // namespace mbgl