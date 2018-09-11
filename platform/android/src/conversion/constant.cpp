#include "constant.hpp"
#include "collection.hpp"

#include <sstream>

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
    std::stringstream sstream;
    sstream << "rgba(" << value.r << ", " << value.g << ", " << value.b << ", "  << value.a << ")";
    return jni::Make<jni::String>(env, sstream.str());
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

} // namespace conversion
} // namespace style
} // namespace mbgl
