#include "polygon.hpp"

#include "../conversion/color.hpp"

namespace mbgl {
namespace android {

mbgl::FillAnnotation Polygon::toAnnotation(jni::JNIEnv& env, const jni::Object<Polygon>& polygon) {
    static auto& javaClass = jni::Class<Polygon>::Singleton(env);
    static auto points = javaClass.GetField<jni::Object<java::util::List>>(env, "points");
    static auto holes = javaClass.GetField<jni::Object<java::util::List>>(env, "holes");
    static auto alpha = javaClass.GetField<float>(env, "alpha");
    static auto fillColor = javaClass.GetField<int>(env, "fillColor");
    static auto strokeColor = javaClass.GetField<int>(env, "strokeColor");

    mbgl::Polygon<double> geometry {
        MultiPoint::toGeometry<mbgl::LinearRing<double>>(env, polygon.Get(env, points))
    };

    auto jHoleListsArray = java::util::List::toArray<java::util::List>(env, polygon.Get(env, holes));

    std::size_t jHoleListsSize = jHoleListsArray.Length(env);
    for (std::size_t i = 0; i < jHoleListsSize; i++) {
        geometry.push_back(MultiPoint::toGeometry<mbgl::LinearRing<double>>(env, jHoleListsArray.Get(env, i)));
    }

    mbgl::FillAnnotation annotation { geometry };
    annotation.opacity = polygon.Get(env, alpha);
    annotation.color = *conversion::convert<mbgl::Color>(env, polygon.Get(env, fillColor));
    annotation.outlineColor = *conversion::convert<mbgl::Color>(env, polygon.Get(env, strokeColor));

    return annotation;
}

void Polygon::registerNative(jni::JNIEnv& env) {
    jni::Class<Polygon>::Singleton(env);
}

} // namespace android
} // namespace mbgl