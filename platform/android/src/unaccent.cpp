#include <unaccent.hpp>
#include <string>
#include "attach_env.hpp"
#include "text/collator_jni.hpp"
#include <jni/jni.hpp>

namespace mbgl {
namespace platform {

std::string unaccent(const std::string& str) {
    android::UniqueEnv env = android::AttachEnv();
    jni::Local<jni::String> input = jni::Make<jni::String>(*env, str);
    jni::Local<jni::String> unaccented = android::StringUtils::unaccent(*env, input);
    return jni::Make<std::string>(*env, unaccented);
}

} // namespace platform
} // namespace mbgl
