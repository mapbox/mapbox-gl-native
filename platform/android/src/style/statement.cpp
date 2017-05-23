#include "statement.hpp"

namespace mbgl {
namespace android {

void Statement::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    Statement::javaClass = *jni::Class<Statement>::Find(env).NewGlobalRef(env).release();
}

jni::Class<Statement> Statement::javaClass;


} // namespace android
} // namespace mbgl