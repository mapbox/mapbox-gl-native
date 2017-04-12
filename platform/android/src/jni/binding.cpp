#include "binding.hpp"

#include <vector>

namespace mbgl {
namespace android {
namespace binding {

#ifndef NDEBUG

using RegisterNativeFunctions = std::vector<void (*)(jni::JNIEnv&)>;
RegisterNativeFunctions& registerNativeFunctions() {
    static RegisterNativeFunctions fns;
    return fns;
}

void RegisterNative(void (*registerNative)(jni::JNIEnv&)) {
    registerNativeFunctions().push_back(registerNative);
}

void RegisterNative(jni::JNIEnv& env) {
    for (auto fn : registerNativeFunctions()) {
        fn(env);
    }
}

#endif

} // namespace binding
} // namespace android
} // namespace mbgl
