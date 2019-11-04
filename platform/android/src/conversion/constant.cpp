#include "constant.hpp"
#include "collection.hpp"
#include "../style/formatted.hpp"

#include <mbgl/util/string.hpp>

namespace mbgl {
namespace android {
namespace conversion {

Result<jni::Local<jni::Object<>>> Converter<jni::Local<jni::Object<>>, bool>::operator()(jni::JNIEnv& env, const bool& value) const {
    return jni::Box(env, value ? jni::jni_true : jni::jni_false);
}

Result<jni::Local<jni::Object<>>> Converter<jni::Local<jni::Object<>>, float>::operator()(jni::JNIEnv& env, const float& value) const {
    return jni::Box(env, value);
}

Result<jni::Local<jni::Object<>>> Converter<jni::Local<jni::Object<>>, double>::operator()(jni::JNIEnv& env, const double& value) const {
    return jni::Box(env, value);
}

Result<jni::Local<jni::Object<>>> Converter<jni::Local<jni::Object<>>, std::string>::operator()(jni::JNIEnv& env, const std::string& value) const {
    return jni::Make<jni::String>(env, value);
}

Result<jni::Local<jni::Object<>>> Converter<jni::Local<jni::Object<>>, Color>::operator()(jni::JNIEnv& env, const Color& value) const {
    return jni::Make<jni::String>(env, value.stringify());
}

Result<jni::Local<jni::Object<>>> Converter<jni::Local<jni::Object<>>, style::expression::Formatted>::operator()(jni::JNIEnv& env, const style::expression::Formatted& value) const {
    return Formatted::New(env, value);
}

Result<jni::Local<jni::Object<>>> Converter<jni::Local<jni::Object<>>, std::vector<std::string>>::operator()(jni::JNIEnv& env, const std::vector<std::string>& value) const {
    auto result = jni::Array<jni::String>::New(env, value.size());

    for (std::size_t i = 0; i < value.size(); i++) {
        result.Set(env, i, jni::Make<jni::String>(env, value.at(i)));
    }

    return result;
}

Result<jni::Local<jni::Object<>>> Converter<jni::Local<jni::Object<>>, std::vector<float>>::operator()(jni::JNIEnv& env, const std::vector<float>& value) const {
    auto result = jni::Array<jni::Float>::New(env, value.size());

    for (std::size_t i = 0; i < value.size(); i++) {
        result.Set(env, i, jni::Box(env, value.at(i)));
    }

    return result;
}

Result<jni::Local<jni::Object<>>> Converter<jni::Local<jni::Object<>>, style::expression::Image>::operator()(
    jni::JNIEnv& env, const style::expression::Image& value) const {
    return jni::Make<jni::String>(env, value.id());
}

} // namespace conversion
} // namespace style
} // namespace mbgl
