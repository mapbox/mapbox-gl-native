#include "util.hpp"

namespace mbgl {
namespace android {
namespace java {
namespace util {

jni::Class<List> List::javaClass;

void registerNative(jni::JNIEnv& env) {
    List::javaClass = *jni::Class<List>::Find(env).NewGlobalRef(env).release();
}


} // namespace util
} // namespace java
} // namespace android
} // namespace mbgl