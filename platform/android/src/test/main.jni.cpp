#include "../jni.hpp"

#include <jni/jni.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/test.hpp>

#include <vector>

namespace {

// Main class (entry point for tests from dalvikvm)
struct Main {
    static constexpr auto Name() {
        return "Main";
    }

    /**
     * JNI Bound to Main#runAllTests()
     */
    static void runAllTests(jni::JNIEnv& env, jni::Object<Main>, jni::Array<jni::String> args) {
        mbgl::Log::Warning(mbgl::Event::JNI, "Starting tests");

        // We need to create a copy of the argv data since Java-internals are stored in UTF-16.
        std::vector<std::string> data;
        // Add a fake first argument to align indices. Google Test expects the first argument to
        // start at index 1; index 0 is the name of the executable.
        data.push_back("main.jar");
        const int argc = args.Length(env);
        for (auto i = 0; i < argc; i++) {
            data.emplace_back(jni::Make<std::string>(env, args.Get(env, i)));
        }

        // Create an array of char pointers that point back to the data array.
        std::vector<const char*> argv;
        for (const auto& arg : data) {
            argv.push_back(arg.data());
        }
        mbgl::runTests(argv.size(), const_cast<char**>(argv.data()));
    }
};

} // namespace

// We're declaring the function, which is libandroid_runtime.so.
// It is defined in AndroidRuntime.cpp:
// https://github.com/android/platform_frameworks_base/blob/master/core/jni/AndroidRuntime.cpp
// Once this symbol goes away, we'll have to revisit.
// This method loads and registers all of the Android-native frameworks so that we can use
// android.util.Log, android.graphics.Bitmap and so on.
// Setting the weak attribute tells the linker that this symbol is loaded at runtime and avoids
// a missing symbol error.
namespace android {
struct AndroidRuntime {
    static int startReg(JNIEnv* env)  __attribute__((weak));
};
} // namespace android

// Main entry point
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    // Load Android-native jni bindings
    jni::JNIEnv& env = jni::GetEnv(*vm, jni::jni_version_1_6);
    android::AndroidRuntime::startReg(&env);

    // Load the main library jni bindings
    mbgl::Log::Info(mbgl::Event::JNI, "Registering main JNI Methods");
    mbgl::android::registerNatives(vm);

    // Load the test library jni bindings
    mbgl::Log::Info(mbgl::Event::JNI, "Registering test JNI Methods");

    jni::RegisterNatives(env, jni::Class<Main>::Find(env),
                         jni::MakeNativeMethod<decltype(Main::runAllTests), &Main::runAllTests>("runAllTests"));

    return JNI_VERSION_1_6;
}
