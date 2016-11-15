#include "../jni.hpp"

#include <EGL/egl.h>
#include <mbgl/gl/extension.hpp>
#include <jni/jni.hpp>
#include <mbgl/test.hpp>

#pragma clang diagnostic ignored "-Wunused-parameter"

void runAllTests(JNIEnv *env, jni::jobject* obj) {
	std::cout << "Starting tests";
	mbgl::runTests(0, nullptr);
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
	std::cout << "Loading tests";

    mbgl::android::theJVM = vm;

    mbgl::gl::InitializeExtensions([] (const char * name) {
        return reinterpret_cast<mbgl::gl::glProc>(eglGetProcAddress(name));
    });

	struct Main { static constexpr auto Name() { return "Main"; } };
	
	jni::JNIEnv& env = jni::GetEnv(*vm, jni::jni_version_1_6);
	
	#define MAKE_NATIVE_METHOD(name, sig) jni::MakeNativeMethod<decltype(name), name>( #name, sig )
	jni::RegisterNatives(env, jni::Class<Main>::Find(env), MAKE_NATIVE_METHOD(runAllTests, "()V"));

	return JNI_VERSION_1_6;
}
