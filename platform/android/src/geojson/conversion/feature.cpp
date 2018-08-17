#include "feature.hpp"

#include "../../gson/json_object.hpp"

namespace mbgl {
namespace android {
namespace conversion {

using namespace gson;

/**
 * Turn feature identifier into std::string
 */
class FeatureIdVisitor {
public:

    template<class T>
    std::string operator()(const T& i) const {
        return std::to_string(i);
    }

    std::string operator()(const std::string& i) const {
        return i;
    }

    std::string operator()(const std::nullptr_t&) const {
        return "";
    }

};

jni::Object<android::geojson::Feature> convertFeature(jni::JNIEnv& env, const mbgl::Feature& value) {
    using namespace mbgl::android::geojson;

    return Feature::fromGeometry(env,
        *jni::SeizeLocal(env, Geometry::New(env, value.geometry)),
        *jni::SeizeLocal(env, JsonObject::New(env, value.properties)),
        *jni::SeizeLocal(env, jni::Make<jni::String>(env, value.id ? value.id.value().match(FeatureIdVisitor()) : "")));
}

Result<jni::Array<jni::Object<android::geojson::Feature>>> Converter<jni::Array<jni::Object<android::geojson::Feature>>, std::vector<mbgl::Feature>>::operator()(jni::JNIEnv& env, const std::vector<mbgl::Feature>& value) const {
    using namespace mbgl::android::geojson;

    auto features = jni::Array<jni::Object<Feature>>::New(env, value.size());

    for (size_t i = 0; i < value.size(); i = i + 1) {
        features.Set(env, i, *jni::SeizeLocal(env, convertFeature(env, value.at(i))));
    }

    return {features};
}

} // namespace conversion
} // namespace android
} // namespace mbgl
