#ifndef MBGL_ANDROID_JNI
#define MBGL_ANDROID_JNI

#include <string>
#include <jni.h>

namespace mbgl {
namespace android {

extern std::string cache_path;
extern std::string data_path;
extern std::string apk_path;

extern jmethodID on_map_changed_id;
extern jmethodID on_fps_changed_id;

extern jclass lon_lat_class;
extern jmethodID lon_lat_constructor_id;
extern jfieldID lon_lat_lon_id;
extern jfieldID lon_lat_lat_id;

extern jclass lon_lat_zoom_class;
extern jmethodID lon_lat_zoom_constructor_id;
extern jfieldID lon_lat_zoom_lon_id;
extern jfieldID lon_lat_zoom_lat_id;
extern jfieldID lon_lat_zoom_zoom_id;

extern jclass runtime_exception_class;
extern jclass null_pointer_exception_class;;

extern jmethodID list_to_array_id;

extern jclass array_list_class;
extern jmethodID array_list_constructor_id;
extern jmethodID array_list_add_id;

}
}

#endif
