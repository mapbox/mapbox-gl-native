#include "feature.hpp"
#include "geometry.hpp"
#include "../gson/json_object.hpp"

#include <mbgl/util/string.hpp>

namespace mbgl {
namespace android {
namespace geojson {

using namespace gson;

mbgl::GeoJSONFeature Feature::convert(jni::JNIEnv& env, const jni::Object<Feature>& jFeature) {
    static auto& javaClass = jni::Class<Feature>::Singleton(env);
    static auto id = javaClass.GetMethod<jni::String ()>(env, "id");
    static auto geometry = javaClass.GetMethod<jni::Object<Geometry> ()>(env, "geometry");
    static auto properties = javaClass.GetMethod<jni::Object<gson::JsonObject> ()>(env, "properties");

    auto jId = jFeature.Call(env, id);

    using mbid = mapbox::feature::identifier;

    return mbgl::GeoJSONFeature{Geometry::convert(env, jFeature.Call(env, geometry)),
                                JsonObject::convert(env, jFeature.Call(env, properties)),
                                jId ? mbid{jni::Make<std::string>(env, jId)} : mbid{mapbox::feature::null_value}};
}

/**
 * Turn feature identifier into std::string
 */
class FeatureIdVisitor {
public:
    template<class T>
    std::string operator()(const T& i) const {
        return util::toString(i);
    }

    std::string operator()(const std::string& i) const {
        return i;
    }

    std::string operator()(const std::nullptr_t&) const {
        return {};
    }

    std::string operator()(const mapbox::feature::null_value_t&) const {
        return {};
    }
};

jni::Local<jni::Object<Feature>> convertFeature(jni::JNIEnv& env, const mbgl::GeoJSONFeature& value) {
    static auto& javaClass = jni::Class<Feature>::Singleton(env);
    static auto method = javaClass.GetStaticMethod<jni::Object<Feature> (jni::Object<Geometry>, jni::Object<JsonObject>, jni::String)>(env, "fromGeometry");

    return javaClass.Call(env, method,
        Geometry::New(env, value.geometry),
        JsonObject::New(env, value.properties),
        jni::Make<jni::String>(env, value.id.is<mbgl::NullValue>() ? std::string {} : value.id.match(FeatureIdVisitor())));
}

jni::Local<jni::Array<jni::Object<Feature>>> Feature::convert(jni::JNIEnv& env, const std::vector<mbgl::Feature>& value) {
    auto features = jni::Array<jni::Object<Feature>>::New(env, value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        features.Set(env, i, convertFeature(env, static_cast<mbgl::GeoJSONFeature>(value.at(i))));
    }

    return features;
}

jni::Local<jni::Array<jni::Object<Feature>>> Feature::convert(jni::JNIEnv& env,
                                                              const std::vector<mbgl::GeoJSONFeature>& value) {
    auto features = jni::Array<jni::Object<Feature>>::New(env, value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        features.Set(env, i, convertFeature(env, value.at(i)));
    }

    return features;
}

void Feature::registerNative(jni::JNIEnv& env) {
    jni::Class<Feature>::Singleton(env);
}

} // namespace geojson
} // namespace android
} // namespace mbgl
