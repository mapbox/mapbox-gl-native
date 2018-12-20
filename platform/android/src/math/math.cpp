#include "math.hpp"

namespace mbgl {
namespace android {
namespace math {

void registerNative(jni::JNIEnv& env) {
    jni::Class<Math>::Singleton(env);
}

} // namespace math
} // namespace android
} // namespace mbgl