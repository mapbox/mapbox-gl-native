#include "runtime.hpp"
#include "../jni.hpp"
#include "../jni_native.hpp"

#include <cassert>
#include <dlfcn.h>
#include <jni.h>
#include <signal.h>
#include <string>

// Required for art / libsigchain
extern "C" JNIEXPORT void EnsureFrontOfChain(int, struct sigaction*) { }
extern "C" JNIEXPORT void AddSpecialSignalHandlerFn(int, void*) { }
extern "C" JNIEXPORT void RemoveSpecialSignalHandlerFn(int, bool (*) (int, siginfo_t*, void*)) { }

namespace {
const std::string kClassPathCommand{"--class_path="};
const std::string kClassPath{"-Djava.class.path="};
const std::string kDefaultDex{"/data/local/tmp/core-tests/classes.dex"};
} // namespace

namespace mbgl {
namespace android {

bool initRuntime(int argc, char *argv[]) {
    void* vmHandle = dlopen("libart.so", RTLD_NOW);
    assert(vmHandle != nullptr);

    using CreateJavaVMFn = jint (*)(JavaVM** vm, JNIEnv** env, void* vmArgs);
    CreateJavaVMFn createJavaVMFn = reinterpret_cast<CreateJavaVMFn>(dlsym(vmHandle, "JNI_CreateJavaVM"));
    assert(createJavaVMFn != nullptr);

    std::string classPath = kClassPath + kDefaultDex;
    for (int i = 0; i < argc; ++i) {
        const std::string arg{argv[i]};
        if (arg.compare(0, kClassPathCommand.length(), kClassPathCommand) == 0) {
            classPath = kClassPath + arg.substr(kClassPathCommand.length());
            break;
        }
    }

    JavaVMOption options[1];
    options[0].optionString = classPath.c_str();

    JavaVMInitArgs args;
    args.version = JNI_VERSION_1_6;
    args.nOptions = 1;
    args.options = options;
    args.ignoreUnrecognized = JNI_FALSE;

    JavaVM* vm;
    JNIEnv* env;

    if (createJavaVMFn(&vm, &env, &args) != JNI_OK) {
        return false;
    }

    void* runtimeHandle = dlopen("libandroid_runtime.so", RTLD_NOW);
    assert(runtimeHandle != nullptr);

    using RegisterNativesFn = jint (*)(JNIEnv* env);
    RegisterNativesFn registerNativesFn = reinterpret_cast<RegisterNativesFn>(dlsym(runtimeHandle, "registerFrameworkNatives"));
    assert(registerNativesFn != nullptr);

    if (registerNativesFn(env) != JNI_OK) {
        return false;
    }

    mbgl::android::registerNatives(vm);
    return true;
}

} // namespace android
} // namespace mbgl
