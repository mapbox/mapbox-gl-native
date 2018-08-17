#include "constant.hpp"

#include <sstream>

namespace mbgl {
namespace android {
namespace conversion {

Result<jni::jobject*> Converter<jni::jobject*, bool>::operator()(jni::JNIEnv& env, const bool& value) const {
    static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/Boolean")).release();
    static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(Z)V");
    return {&jni::NewObject(env, *javaClass, *constructor, (jboolean) value)};
}

Result<jni::jboolean> Converter<jni::jboolean, bool>::operator()(jni::JNIEnv&, const bool& value) const {
    return {(jni::jboolean) value};
}

Result<jni::jobject*> Converter<jni::jobject*, float>::operator()(jni::JNIEnv& env, const float& value) const {
    static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/Float")).release();
    static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(F)V");
    return {&jni::NewObject(env, *javaClass, *constructor, (jfloat) value)};
}

Result<jni::jfloat> Converter<jni::jfloat, float>::operator()(jni::JNIEnv&, const float& value) const {
    return {(jni::jfloat) value};
}

Result<jni::jobject*> Converter<jni::jobject*, double>::operator()(jni::JNIEnv& env, const double& value) const {
    static jni::jclass* javaClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/Double")).release();
    static jni::jmethodID* constructor = &jni::GetMethodID(env, *javaClass, "<init>", "(D)V");
    return {&jni::NewObject(env, *javaClass, *constructor, (jfloat) value)};
}

Result<jni::jdouble> Converter<jni::jdouble, float>::operator()(jni::JNIEnv&, const double& value) const {
    return {(jni::jdouble) value};
}

Result<jni::jobject*> Converter<jni::jobject*, std::string>::operator()(jni::JNIEnv& env, const std::string& value) const {
    return {jni::Make<jni::String>(env, value).Get()};
}

Result<jni::jstring*> Converter<jni::jstring*, std::string>::operator()(jni::JNIEnv& env, const std::string& value) const {
    return {jni::Make<jni::String>(env, value).Get()};
}

Result<jni::jobject*> Converter<jni::jobject*, Color>::operator()(jni::JNIEnv& env, const Color& value) const {
    std::stringstream sstream;
    sstream << "rgba(" << value.r << ", " << value.g << ", " << value.b << ", "  << value.a << ")";
    std::string result = sstream.str();
    return convert<jni::jobject*, std::string>(env, result);
}

Result<jni::jobject*> Converter<jni::jobject*, std::vector<std::string>>::operator()(jni::JNIEnv& env, const std::vector<std::string>& value) const {
    static jni::jclass* stringCass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/String")).release();
    jni::jarray<jni::jobject>& jarray = jni::NewObjectArray(env, value.size(), *stringCass);

    for(size_t i = 0; i < value.size(); i = i + 1) {
        Result<jni::jobject*> converted = convert<jni::jobject*, std::string>(env, value.at(i));
        jni::SetObjectArrayElement(env, jarray, i, *converted);
    }

    return &jarray;
}

Result<jni::jobject*> Converter<jni::jobject*, std::vector<float>>::operator()(jni::JNIEnv& env, const std::vector<float>& value) const {
    static jni::jclass* floatClass = jni::NewGlobalRef(env, &jni::FindClass(env, "java/lang/Float")).release();
    jni::jarray<jni::jobject>& jarray = jni::NewObjectArray(env, value.size(), *floatClass);

    for(size_t i = 0; i < value.size(); i = i + 1) {
        Result<jni::jobject*> converted = convert<jni::jobject*, float>(env, value.at(i));
        jni::SetObjectArrayElement(env, jarray, i, *converted);
    }

    return &jarray;
}

// Java -> C++

Result<std::string> Converter<std::string, jni::String>::operator()(jni::JNIEnv& env, const jni::String& value) const {
    return { jni::Make<std::string>(env, value) };
}

} // namespace conversion
} // namespace style
} // namespace mbgl
