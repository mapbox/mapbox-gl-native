#include "util.hpp"

namespace mbgl {
namespace android {
namespace java {
namespace util {

jni::Class<List> List::javaClass;
jni::Class<Set> Set::javaClass;
jni::Class<Map> Map::javaClass;
jni::Class<Map::Entry> Map::Entry::javaClass;

void registerNative(jni::JNIEnv& env) {
    List::javaClass = *jni::Class<List>::Find(env).NewGlobalRef(env).release();
    Set::javaClass = *jni::Class<Set>::Find(env).NewGlobalRef(env).release();
    Map::javaClass = *jni::Class<Map>::Find(env).NewGlobalRef(env).release();
    Map::Entry::javaClass = *jni::Class<Map::Entry>::Find(env).NewGlobalRef(env).release();
}


} // namespace util
} // namespace java
} // namespace android
} // namespace mbgl