#pragma once

#include <string>
#include <jni/jni.hpp>

typedef struct _jmethodID* jmethodID;
typedef struct _JavaVM JavaVM;
typedef struct _JNIEnv JNIEnv;

namespace mbgl {
namespace android {

extern JavaVM* theJVM;

extern std::string cachePath;
extern std::string dataPath;
extern std::string apkPath;
extern std::string androidRelease;

bool attach_jni_thread(JavaVM* vm, JNIEnv** env, std::string threadName);
void detach_jni_thread(JavaVM* vm, JNIEnv** env, bool detach);

extern void registerNatives(JavaVM* vm);

template<typename F>
void attachThreadAndExecute(F&& function);

} // namespace android
} // namespace mbgl
