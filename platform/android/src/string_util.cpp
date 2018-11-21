#include <mbgl/util/platform.hpp>
#include "attach_env.hpp"
#include <jni/jni.hpp>

namespace mbgl {
namespace platform {

std::string uppercase(const std::string& str) {
    auto env{ android::AttachEnv() };
    jni::Local<jni::String> value = jni::Make<jni::String>(*env, str.c_str());
    static auto toUpperCase = jni::Class<jni::StringTag>::Singleton(*env).GetMethod<jni::String()>(*env, "toUpperCase");
    auto result = value.Call(*env, toUpperCase);
    return jni::Make<std::string>(*env, result);
}

std::string lowercase(const std::string& str) {
    auto env{ android::AttachEnv() };
    jni::Local<jni::String> value = jni::Make<jni::String>(*env, str.c_str());
    static auto toLowerCase = jni::Class<jni::StringTag>::Singleton(*env).GetMethod<jni::String()>(*env, "toLowerCase");
    auto result = value.Call(*env, toLowerCase);
    return jni::Make<std::string>(*env, result);
}

} // namespace platform
} // namespace mbgl
