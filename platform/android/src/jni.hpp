#pragma once

#include <string>

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

extern jmethodID onInvalidateId;
extern jmethodID wakeCallbackId;
extern jmethodID onMapChangedId;
extern jmethodID onFpsChangedId;
extern jmethodID onSnapshotReadyId;

extern bool attach_jni_thread(JavaVM* vm, JNIEnv** env, std::string threadName);
extern void detach_jni_thread(JavaVM* vm, JNIEnv** env, bool detach);

extern void registerNatives(JavaVM* vm);

} // namespace android
} // namespace mbgl
