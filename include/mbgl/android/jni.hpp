#ifndef MBGL_ANDROID_JNI
#define MBGL_ANDROID_JNI

#include <string>

// Forward definition of JNI types
typedef class _jclass* jclass;
typedef struct _jmethodID* jmethodID;
typedef struct _jfieldID* jfieldID;

namespace mbgl {
namespace android {

extern std::string cachePath;
extern std::string dataPath;
extern std::string apkPath;
extern std::string androidRelease;

extern jmethodID onMapChangedId;
extern jmethodID onFpsChangedId;

extern jclass lonLatClass;
extern jmethodID lonLatConstructorId;
extern jfieldID lonLatLonId;
extern jfieldID lonLatLatId;

extern jclass lonLatZoomClass;
extern jmethodID lonLatZoomConstructorId;
extern jfieldID lonLatZoomLonId;
extern jfieldID lonLatZoomLatId;
extern jfieldID lonLatZoomZoomId;

extern jclass runtimeExceptionClass;
extern jclass nullPointerExceptionClass;

extern jmethodID listToArrayId;

extern jclass arrayListClass;
extern jmethodID arrayListConstructorId;
extern jmethodID arrayListAddId;

}
}

#endif
