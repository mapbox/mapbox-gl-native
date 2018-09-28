#include "formatted_section.hpp"

namespace mbgl {
namespace android {

void FormattedSection::registerNative(jni::JNIEnv& env) {
    jni::Class<FormattedSection>::Singleton(env);
}

} // namespace android
} // namespace mbgl