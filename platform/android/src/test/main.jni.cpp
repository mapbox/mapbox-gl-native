#include "../jni.hpp"

#include <android/log.h>
#include <jni/jni.hpp>
#include <jni/jni.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/test.hpp>

#include <vector>

#pragma clang diagnostic ignored "-Wunused-parameter"

#define MAKE_NATIVE_METHOD(name, sig) jni::MakeNativeMethod<decltype(name), name>( #name, sig )

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

// JNI Bindings to stub the android.util.Log implementation

static jboolean isLoggable(JNIEnv* env, jni::jobject* clazz, jni::jstring* tag, jint level) {
    return true;
}

static jint println_native(JNIEnv* env, jni::jobject* clazz, jint bufID, jint priority, jni::jstring* jtag, jni::jstring* jmessage) {
    if (jtag == nullptr || jmessage == nullptr) {
        return false;
    }

    std::string tag = jni::Make<std::string>(*env, jni::String(jtag));
    std::string message = jni::Make<std::string>(*env, jni::String(jmessage));

    return __android_log_print(priority, tag.c_str(), "%s", message.c_str());
}

static jint logger_entry_max_payload_native(JNIEnv* env, jni::jobject* clazz) {
    return static_cast<jint>(4068);
}


// Main entry point

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    // Load the main library jni bindings
    mbgl::Log::Info(mbgl::Event::JNI, "Registering main JNI Methods");
    mbgl::android::registerNatives(vm);

    // Load the test library jni bindings
    mbgl::Log::Info(mbgl::Event::JNI, "Registering test JNI Methods");

    jni::JNIEnv& env = jni::GetEnv(*vm, jni::jni_version_1_6);

    jni::RegisterNatives(env, jni::Class<Main>::Find(env),
                         jni::MakeNativeMethod<decltype(Main::runAllTests), &Main::runAllTests>("runAllTests"));

    // Bindings for system classes
    struct Log { static constexpr auto Name() { return "android/util/Log"; } };
    try {
        jni::RegisterNatives(env, jni::Class<Log>::Find(env),
            MAKE_NATIVE_METHOD(isLoggable, "(Ljava/lang/String;I)Z"),
            MAKE_NATIVE_METHOD(logger_entry_max_payload_native, "()I"),
            MAKE_NATIVE_METHOD(println_native, "(IILjava/lang/String;Ljava/lang/String;)I")
        );
    } catch (jni::PendingJavaException ex) {
        env.ThrowNew(jni::JavaErrorClass(env), "Could not register Log mocks");
    }

    return JNI_VERSION_1_6;
}
