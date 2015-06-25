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

extern jmethodID onInvalidateId;
extern jmethodID onMapChangedId;
extern jmethodID onFpsChangedId;

extern jclass latLngClass;
extern jmethodID latLngConstructorId;
extern jfieldID latLngLatitudeId;
extern jfieldID latLngLongitudeId;

extern jclass latLngZoomClass;
extern jmethodID latLngZoomConstructorId;
extern jfieldID latLngZoomLatitudeId;
extern jfieldID latLngZoomLongitudeId;
extern jfieldID latLngZoomZoomId;

extern jclass runtimeExceptionClass;
extern jclass nullPointerExceptionClass;

extern jmethodID listToArrayId;

extern jclass arrayListClass;
extern jmethodID arrayListConstructorId;
extern jmethodID arrayListAddId;

extern jclass projectedMetersClass;
extern jmethodID projectedMetersConstructorId;
extern jfieldID projectedMetersNorthingId;
extern jfieldID projectedMetersEastingId;

extern jclass pointFClass;
extern jmethodID pointFConstructorId;
extern jfieldID pointFXId;
extern jfieldID pointFYId;

}
}

#endif
