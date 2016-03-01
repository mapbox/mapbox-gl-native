#ifndef MBGL_ANDROID_JNI
#define MBGL_ANDROID_JNI

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
extern jmethodID onMapChangedId;
extern jmethodID onFpsChangedId;

extern bool attach_jni_thread(JavaVM* vm, JNIEnv** env, std::string threadName);
extern void detach_jni_thread(JavaVM* vm, JNIEnv** env, bool detach);

}
}

#endif
