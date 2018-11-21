#include "json_primitive.hpp"

namespace mbgl {
namespace android {
namespace gson {

void JsonPrimitive::registerNative(jni::JNIEnv &env) {
    jni::Class<JsonPrimitive>::Singleton(env);
}

} // namespace gson
} // namespace android
} // namespace mbgl