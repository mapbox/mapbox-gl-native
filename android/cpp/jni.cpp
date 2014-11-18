#include <cstdint>
#include <cinttypes>

#include <string>
#include <locale>
#include <codecvt>
#include <array>
#include <vector>

#include <jni.h>

#include <android/native_window_jni.h>

#include <coffeecatch/coffeejni.h>

#include <mbgl/android/jni.hpp>
#include <mbgl/android/native_map_view.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/platform/android/log_android.hpp>
#include <mbgl/platform/event.hpp>
#include <mbgl/platform/log.hpp>

#pragma clang diagnostic ignored "-Wunused-parameter"

namespace mbgl {
namespace android {

std::string cache_path;
std::string data_path;
std::string apk_path;

jmethodID on_map_changed_id = nullptr;

jclass lon_lat_class = nullptr;
jmethodID lon_lat_constructor_id = nullptr;
jfieldID lon_lat_lon_id = nullptr;
jfieldID lon_lat_lat_id = nullptr;

jclass lon_lat_zoom_class = nullptr;
jmethodID lon_lat_zoom_constructor_id = nullptr;
jfieldID lon_lat_zoom_lon_id = nullptr;
jfieldID lon_lat_zoom_lat_id = nullptr;
jfieldID lon_lat_zoom_zoom_id = nullptr;

jclass runtime_exception_class = nullptr;
jclass null_pointer_exception_class = nullptr;

jmethodID list_to_array_id = nullptr;

jclass array_list_class = nullptr;
jmethodID array_list_constructor_id = nullptr;
jmethodID array_list_add_id = nullptr;

bool throw_error(JNIEnv* env, const char* msg) {
    if (env->ThrowNew(runtime_exception_class, msg) < 0) {
        env->ExceptionDescribe();
        return false;
    }

    return true;
}

std::string std_string_from_jstring(JNIEnv* env, jstring jstr) {
    std::string str;

    if (jstr == nullptr) {
        if (env->ThrowNew(null_pointer_exception_class, "String cannot be null.") < 0) {
            env->ExceptionDescribe();
            return str;
        }

        return str;
    }

    jsize len = env->GetStringLength(jstr);
    if (len < 0) {
        env->ExceptionDescribe();
        return str;
    }

    const jchar* chars = env->GetStringChars(jstr, nullptr);
    if (chars == nullptr) {
        env->ExceptionDescribe();
        return str;
    }

    std::u16string ustr(reinterpret_cast<const char16_t*>(chars), len);
    env->ReleaseStringChars(jstr, chars);
    chars = nullptr;
    str = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t>().to_bytes(ustr);
    return str;
}

jstring std_string_to_jstring(JNIEnv* env, std::string str) {
    std::u16string ustr = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t>().from_bytes(str);

    jstring jstr = env->NewString(reinterpret_cast<const jchar*>(ustr.c_str()), ustr.size());
    if (jstr == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return jstr;
}

std::vector<std::string> std_vector_string_from_jobject(JNIEnv* env, jobject jlist) {
    std::vector<std::string> vector;

    if (jlist == nullptr) {
        if (env->ThrowNew(null_pointer_exception_class, "List cannot be null.") < 0) {
            env->ExceptionDescribe();
            return vector;
        }

        return vector;
    }

    jobjectArray array = reinterpret_cast<jobjectArray>(env->CallObjectMethod(jlist, list_to_array_id));
    if (env->ExceptionCheck() || (array == nullptr)) {
        env->ExceptionDescribe();
        return vector;
    }

    jsize len = env->GetArrayLength(array);
    if (len < 0) {
        env->ExceptionDescribe();
        return vector;
    }

    for (jsize i = 0; i < len; i++) {
        jstring jstr = reinterpret_cast<jstring>(env->GetObjectArrayElement(array, i));
        if (jstr == nullptr) {
            env->ExceptionDescribe();
            return vector;
        }

        vector.push_back(std_string_from_jstring(env, jstr));
    }

    return vector;
}

jobject std_vector_string_to_jobject(JNIEnv* env, std::vector<std::string> vector)  {
    jobject jlist = env->NewObject(array_list_class, array_list_constructor_id);
    if (jlist == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    for (std::string str : vector) {
        env->CallBooleanMethod(jlist, array_list_add_id, std_string_to_jstring(env, str));
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }
    }

    return jlist;
}

}
}

namespace {

using namespace mbgl::android;

jlong JNICALL nativeCreate(JNIEnv* env, jobject obj, jstring cache_path_, jstring data_path_, jstring apk_path_) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCreate");
    cache_path = std_string_from_jstring(env, cache_path_);
    data_path = std_string_from_jstring(env, data_path_);
    apk_path = std_string_from_jstring(env, apk_path_);
    NativeMapView* native_map_view = nullptr;
    COFFEE_TRY_JNI(env, native_map_view = new NativeMapView(env, obj));
    if (native_map_view == nullptr) {
        throw_error(env, "Unable to create NativeMapView.");
        return 0;
    }
    jlong map_view_ptr = reinterpret_cast<jlong>(native_map_view);
    return map_view_ptr;
}

void JNICALL nativeDestroy(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeDestroy");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    delete native_map_view;
    native_map_view = nullptr;
}

void JNICALL nativeInitializeDisplay(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeInitializeDisplay");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    bool ret = false;
    COFFEE_TRY_JNI(env, ret = native_map_view->initializeDisplay());
    if (!ret) {
        throw_error(env, "Unable to initialize GL display.");
    }
}

void JNICALL nativeTerminateDisplay(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeTerminateDisplay");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->terminateDisplay());
}

void JNICALL nativeInitializeContext(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeInitializeContext");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    bool ret = false;
    COFFEE_TRY_JNI(env, ret = native_map_view->initializeContext());
    if (!ret) {
        throw_error(env, "Unable to initialize GL context.");
    }
}

void JNICALL nativeTerminateContext(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeTerminateContext");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->terminateContext());
}

void JNICALL nativeCreateSurface(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jobject surface) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCreateSurface");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    bool ret = false;
    COFFEE_TRY_JNI(env, ret = native_map_view->createSurface(ANativeWindow_fromSurface(env, surface)));
    if (!ret) {
        throw_error(env, "Unable to create GL surface.");
    }
}

void JNICALL nativeDestroySurface(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeDestroySurface");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->destroySurface());
}

void JNICALL nativeStart(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeStart");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->start());
}

void JNICALL nativeStop(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeStop");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->stop());
}

void JNICALL nativePause(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativePause");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->pause());
}

void JNICALL nativeResume(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResume");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->resume());
}

void JNICALL nativeRun(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeRun");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().run());
}

void JNICALL nativeRerender(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeRerender");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().rerender());
}

void JNICALL nativeUpdate(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeUpdate");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().update());
}

void JNICALL nativeCleanup(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCleanup");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().cleanup());
}

void JNICALL nativeTerminate(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeTerminate");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().terminate());
}

jboolean JNICALL nativeNeedsSwap(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeNeedsSwap");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    jboolean ret = false;
    COFFEE_TRY_JNI(env, ret = native_map_view->getMap().needsSwap());
    return ret;
}

void JNICALL nativeSwapped(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSwapped");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, (native_map_view->getMap().swapped()));
}

void JNICALL nativeResize(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jint width, jint height, jfloat ratio) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResize");
    assert(native_map_view_ptr != 0);
    assert(width >= 0);
    assert(height >= 0);
    assert(width <= UINT16_MAX);
    assert(height <= UINT16_MAX);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().resize(width, height, ratio));
}

void JNICALL nativeResize(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jint width, jint height, jfloat ratio, jint fb_width, jint fb_height) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResize");
    assert(native_map_view_ptr != 0);
    assert(width >= 0);
    assert(height >= 0);
    assert(width <= UINT16_MAX);
    assert(height <= UINT16_MAX);
    assert(fb_width >= 0);
    assert(fb_height >= 0);
    assert(fb_width <= UINT16_MAX);
    assert(fb_height <= UINT16_MAX);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().resize(width, height, ratio, fb_width, fb_height));
}

void JNICALL nativeSetAppliedClasses(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jobject classes) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetAppliedClasses");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().setAppliedClasses(std_vector_string_from_jobject(env, classes)));
}

jobject JNICALL nativeGetAppliedClasses(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetAppliedClasses");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    jobject ret = nullptr;
    COFFEE_TRY_JNI(env, ret = std_vector_string_to_jobject(env, native_map_view->getMap().getAppliedClasses()));
    return ret;
}

void JNICALL nativeSetDefaultTransitionDuration(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jlong duration_milliseconds) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetDefaultTransitionDuration");
    assert(native_map_view_ptr != 0);
    assert(duration_milliseconds >= 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().setDefaultTransitionDuration(duration_milliseconds));
}

void JNICALL nativeSetStyleURL(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jstring url) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetStyleURL");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().setStyleURL(std_string_from_jstring(env, url)));
}

void JNICALL nativeSetStyleJSON(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jstring new_style_json, jstring base) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetStyleJSON");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().setStyleJSON(std_string_from_jstring(env, new_style_json), std_string_from_jstring(env, base)));
}

jstring JNICALL nativeGetStyleJSON(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetStyleJSON");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    jstring ret = nullptr;
    COFFEE_TRY_JNI(env, ret = std_string_to_jstring(env, native_map_view->getMap().getStyleJSON()));
    return ret;
}

void JNICALL nativeSetAccessToken(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jstring access_token) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetAccessToken");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().setAccessToken(std_string_from_jstring(env, access_token)));
}

jstring JNICALL nativeGetAccessToken(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetAccessToken");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    jstring ret = nullptr;
    COFFEE_TRY_JNI(env, ret = std_string_to_jstring(env, native_map_view->getMap().getAccessToken()));
    return ret;
}

void JNICALL nativeCancelTransitions(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCancelTransitions");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().cancelTransitions());
}

void JNICALL nativeMoveBy(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble dx, jdouble dy, jdouble duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeMoveBy");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().moveBy(dx, dy, duration));
}

void JNICALL nativeSetLonLat(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jobject lon_lat, jdouble duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetLonLat");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);

    double lon = env->GetDoubleField(lon_lat, lon_lat_lon_id);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    double lat = env->GetDoubleField(lon_lat, lon_lat_lat_id);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    COFFEE_TRY_JNI(env, native_map_view->getMap().setLonLat(lon, lat, duration));
}

jobject JNICALL nativeGetLonLat(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetLonLat");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    double lon = 0.0, lat = 0.0;
    COFFEE_TRY_JNI(env, native_map_view->getMap().getLonLat(lon, lat));

    jobject ret = env->NewObject(lon_lat_class, lon_lat_constructor_id, lon, lat);
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}

void JNICALL nativeStartPanning(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeStartPanning");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().startPanning());
}

void JNICALL nativeStopPanning(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeStopPanning");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().stopPanning());
}

void JNICALL nativeResetPosition(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResetPosition");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().resetPosition());
}

void JNICALL nativeScaleBy(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble ds, jdouble cx, jdouble cy, jdouble duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeScaleBy");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().scaleBy(ds, cx, cy, duration));
}

void JNICALL nativeSetScale(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble scale, jdouble cx, jdouble cy, jdouble duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetScale");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().setScale(scale, cx, cy, duration));
}

jdouble JNICALL nativeGetScale(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetScale");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    jdouble ret = 0.0;
    COFFEE_TRY_JNI(env, ret = native_map_view->getMap().getScale());
    return ret;
}

void JNICALL nativeSetZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble zoom, jdouble duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetZoom");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().setZoom(zoom, duration));
}

jdouble JNICALL nativeGetZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetZoom");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    jdouble ret = 0.0;
    COFFEE_TRY_JNI(env, ret = native_map_view->getMap().getZoom());
    return ret;
}

void JNICALL nativeSetLonLatZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jobject lon_lat_zoom, jdouble duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetLonLatZoom");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);

    double lon = env->GetDoubleField(lon_lat_zoom, lon_lat_zoom_lon_id);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    double lat = env->GetDoubleField(lon_lat_zoom, lon_lat_zoom_lat_id);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    double zoom = env->GetDoubleField(lon_lat_zoom, lon_lat_zoom_zoom_id);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    COFFEE_TRY_JNI(env, native_map_view->getMap().setLonLatZoom(lon, lat, zoom, duration));
}

jobject JNICALL nativeGetLonLatZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetLonLatZoom");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    double lon = 0.0, lat = 0.0, zoom = 0.0;
    COFFEE_TRY_JNI(env, native_map_view->getMap().getLonLatZoom(lon, lat, zoom));

    jobject ret = env->NewObject(lon_lat_zoom_class, lon_lat_zoom_constructor_id, lon, lat, zoom);
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}

void JNICALL nativeResetZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResetZoom");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().resetZoom());
}

void JNICALL nativeStartScaling(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeStartScaling");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().startScaling());
}

void JNICALL nativeStopScaling(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeStopScaling");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().stopScaling());
}

jdouble JNICALL nativeGetMinZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetMinZoom");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    jdouble ret = 0.0;
    COFFEE_TRY_JNI(env, ret = native_map_view->getMap().getMinZoom());
    return ret;
}

jdouble JNICALL nativeGetMaxZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetMaxZoom");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    jdouble ret = 0.0;
    COFFEE_TRY_JNI(env, ret = native_map_view->getMap().getMaxZoom());
    return ret;
}

void JNICALL nativeRotateBy(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble sx, jdouble sy, jdouble ex, jdouble ey, jdouble duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeRotateBy");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().rotateBy(sx, sy, ex, ey, duration));
}

void JNICALL nativeSetBearing(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble degrees, jdouble duration) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetBearing");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().setBearing(degrees, duration));
}

void JNICALL nativeSetBearing(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble degrees, jdouble cx, jdouble cy) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetBearing");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().setBearing(degrees, cx, cy));
}

jdouble JNICALL nativeGetBearing(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetBearing");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    jdouble ret = 0.0;
    COFFEE_TRY_JNI(env, ret = native_map_view->getMap().getBearing());
    return ret;
}

void JNICALL nativeResetNorth(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeResetNorth");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().resetNorth());
}

void JNICALL nativeStartRotating(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeStartRotating");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().startRotating());
}

void JNICALL nativeStopRotating(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeStopRotating");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().stopRotating());
}

jboolean JNICALL nativeCanRotate(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeCanRotate");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    jboolean ret = false;
    COFFEE_TRY_JNI(env, ret = native_map_view->getMap().canRotate());
    return ret;
}

void JNICALL nativeSetDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jboolean debug) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetDebug");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().setDebug(debug));
}

void JNICALL nativeToggleDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeToggleDebug");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().toggleDebug());
}

jboolean JNICALL nativeGetDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeGetDebug");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    jboolean ret = false;
    COFFEE_TRY_JNI(env, ret = native_map_view->getMap().getDebug());
    return ret;
}

void JNICALL nativeSetReachability(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jboolean status) {
    mbgl::Log::Debug(mbgl::Event::JNI, "nativeSetReachability");
    assert(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    COFFEE_TRY_JNI(env, native_map_view->getMap().setReachability(status));
}

}

extern "C" {

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    mbgl::Log::Set<mbgl::AndroidLogBackend>();

    mbgl::Log::Debug(mbgl::Event::JNI, "JNI_OnLoad");

    JNIEnv* env = nullptr;
    jint ret = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
        return JNI_ERR;
    }

    lon_lat_class = env->FindClass("com/mapbox/mapboxgl/lib/LonLat");
    if (lon_lat_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_lat_constructor_id = env->GetMethodID(lon_lat_class, "<init>", "(DD)V");
    if (lon_lat_constructor_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_lat_lon_id = env->GetFieldID(lon_lat_class, "lon", "D");
    if (lon_lat_lon_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_lat_lat_id = env->GetFieldID(lon_lat_class, "lat", "D");
    if (lon_lat_lat_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_lat_zoom_class = env->FindClass("com/mapbox/mapboxgl/lib/LonLatZoom");
    if (lon_lat_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_lat_zoom_constructor_id = env->GetMethodID(lon_lat_zoom_class, "<init>", "(DDD)V");
    if (lon_lat_zoom_constructor_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_lat_zoom_lon_id = env->GetFieldID(lon_lat_zoom_class, "lon", "D");
    if (lon_lat_zoom_lon_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_lat_zoom_lat_id = env->GetFieldID(lon_lat_zoom_class, "lat", "D");
    if (lon_lat_zoom_lat_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_lat_zoom_zoom_id = env->GetFieldID(lon_lat_zoom_class, "zoom", "D");
    if (lon_lat_zoom_zoom_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    jclass native_map_view_class = env->FindClass("com/mapbox/mapboxgl/lib/NativeMapView");
    if (native_map_view_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    on_map_changed_id = env->GetMethodID(native_map_view_class, "onMapChanged", "()V");
    if (on_map_changed_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    runtime_exception_class = env->FindClass("java/lang/RuntimeException");
    if (runtime_exception_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    null_pointer_exception_class = env->FindClass("java/lang/NullPointerException");
    if (null_pointer_exception_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    jclass list_class = env->FindClass("java/util/List");
    if (list_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    list_to_array_id = env->GetMethodID(list_class, "toArray", "()[Ljava/lang/Object;");
    if (list_to_array_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    array_list_class = env->FindClass("java/util/ArrayList");
    if (array_list_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    array_list_constructor_id = env->GetMethodID(array_list_class, "<init>", "()V");
    if (array_list_constructor_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    array_list_add_id = env->GetMethodID(array_list_class, "add", "(Ljava/lang/Object;)Z");
    if (array_list_add_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    // NOTE: if you get java.lang.UnsatisfiedLinkError you likely forgot to set the size of the array correctly (too large)
    std::array<JNINativeMethod, 60> methods = {{ // Can remove the extra brace in C++14
        { "nativeCreate", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J", reinterpret_cast<void*>(&nativeCreate) },
        { "nativeDestroy", "(J)V", reinterpret_cast<void*>(&nativeDestroy) },
        { "nativeInitializeDisplay", "(J)V", reinterpret_cast<void*>(&nativeInitializeDisplay) },
        { "nativeTerminateDisplay", "(J)V", reinterpret_cast<void*>(&nativeTerminateDisplay) },
        { "nativeInitializeContext", "(J)V", reinterpret_cast<void*>(&nativeInitializeContext) },
        { "nativeTerminateContext", "(J)V", reinterpret_cast<void*>(&nativeTerminateContext) },
        { "nativeCreateSurface", "(JLandroid/view/Surface;)V", reinterpret_cast<void*>(&nativeCreateSurface) },
        { "nativeDestroySurface", "(J)V", reinterpret_cast<void*>(&nativeDestroySurface) },
        { "nativeStart", "(J)V", reinterpret_cast<void*>(&nativeStart) },
        { "nativeStop", "(J)V", reinterpret_cast<void*>(&nativeStop) },
        { "nativePause", "(J)V", reinterpret_cast<void*>(&nativePause) },
        { "nativeResume", "(J)V", reinterpret_cast<void*>(&nativeResume) },
        { "nativeRun", "(J)V", reinterpret_cast<void*>(&nativeRun) },
        { "nativeRerender", "(J)V", reinterpret_cast<void*>(&nativeRerender) },
        { "nativeUpdate", "(J)V", reinterpret_cast<void*>(&nativeUpdate) },
        { "nativeCleanup", "(J)V", reinterpret_cast<void*>(&nativeCleanup) },
        { "nativeTerminate", "(J)V", reinterpret_cast<void*>(&nativeTerminate) },
        { "nativeNeedsSwap", "(J)Z", reinterpret_cast<void*>(&nativeNeedsSwap) },
        { "nativeSwapped", "(J)V", reinterpret_cast<void*>(&nativeSwapped) },
        { "nativeResize", "(JIIF)V", reinterpret_cast<void*>(static_cast<void JNICALL(*)(JNIEnv*,jobject,jlong,jint,jint,jfloat)>(&nativeResize)) },
        { "nativeResize", "(JIIFII)V", reinterpret_cast<void*>(static_cast<void JNICALL(*)(JNIEnv*,jobject,jlong,jint,jint,jfloat,jint,jint)>(&nativeResize)) },
        { "nativeSetAppliedClasses", "(JLjava/util/List;)V", reinterpret_cast<void*>(&nativeSetAppliedClasses) },
        { "nativeGetAppliedClasses", "(J)Ljava/util/List;", reinterpret_cast<void*>(&nativeGetAppliedClasses) },
        { "nativeSetDefaultTransitionDuration", "(JJ)V", reinterpret_cast<void*>(&nativeSetDefaultTransitionDuration) },
        { "nativeSetStyleUrl", "(JLjava/lang/String;)V", reinterpret_cast<void*>(&nativeSetStyleURL) },
        { "nativeSetStyleJson", "(JLjava/lang/String;Ljava/lang/String;)V", reinterpret_cast<void*>(&nativeSetStyleJSON) },
        { "nativeGetStyleJson", "(J)Ljava/lang/String;", reinterpret_cast<void*>(&nativeGetStyleJSON) },
        { "nativeSetAccessToken", "(JLjava/lang/String;)V", reinterpret_cast<void*>(&nativeSetAccessToken) },
        { "nativeGetAccessToken", "(J)Ljava/lang/String;", reinterpret_cast<void*>(&nativeGetAccessToken) },
        { "nativeCancelTransitions", "(J)V", reinterpret_cast<void*>(&nativeCancelTransitions) },
        { "nativeMoveBy", "(JDDD)V", reinterpret_cast<void*>(&nativeMoveBy) },
        { "nativeSetLonLat", "(JLcom/mapbox/mapboxgl/lib/LonLat;D)V", reinterpret_cast<void*>(&nativeSetLonLat) },
        { "nativeGetLonLat", "(J)Lcom/mapbox/mapboxgl/lib/LonLat;", reinterpret_cast<void*>(&nativeGetLonLat) },
        { "nativeStartPanning", "(J)V", reinterpret_cast<void*>(&nativeStartPanning) },
        { "nativeStopPanning", "(J)V", reinterpret_cast<void*>(&nativeStopPanning) },
        { "nativeResetPosition", "(J)V", reinterpret_cast<void*>(&nativeResetPosition) },
        { "nativeScaleBy", "(JDDDD)V", reinterpret_cast<void*>(&nativeScaleBy) },
        { "nativeSetScale", "(JDDDD)V", reinterpret_cast<void*>(&nativeSetScale) },
        { "nativeGetScale", "(J)D", reinterpret_cast<void*>(&nativeGetScale) },
        { "nativeSetZoom", "(JDD)V", reinterpret_cast<void*>(&nativeSetZoom) },
        { "nativeGetZoom", "(J)D", reinterpret_cast<void*>(&nativeGetZoom) },
        { "nativeSetLonLatZoom", "(JLcom/mapbox/mapboxgl/lib/LonLatZoom;D)V", reinterpret_cast<void*>(&nativeSetLonLatZoom) },
        { "nativeGetLonLatZoom", "(J)Lcom/mapbox/mapboxgl/lib/LonLatZoom;", reinterpret_cast<void*>(&nativeGetLonLatZoom) },
        { "nativeResetZoom", "(J)V", reinterpret_cast<void*>(&nativeResetZoom) },
        { "nativeStartPanning", "(J)V", reinterpret_cast<void*>(&nativeStartScaling) },
        { "nativeStopPanning", "(J)V", reinterpret_cast<void*>(&nativeStopScaling) },
        { "nativeGetMinZoom", "(J)D", reinterpret_cast<void*>(&nativeGetMinZoom) },
        { "nativeGetMaxZoom", "(J)D", reinterpret_cast<void*>(&nativeGetMaxZoom) },
        { "nativeRotateBy", "(JDDDDD)V", reinterpret_cast<void*>(&nativeRotateBy) },
        { "nativeSetBearing", "(JDD)V", reinterpret_cast<void*>(static_cast<void JNICALL(*)(JNIEnv*,jobject,jlong,jdouble,jdouble)>(&nativeSetBearing)) },
        { "nativeSetBearing", "(JDDD)V", reinterpret_cast<void*>(static_cast<void JNICALL(*)(JNIEnv*,jobject,jlong,jdouble,jdouble,jdouble)>(&nativeSetBearing)) },
        { "nativeGetBearing", "(J)D", reinterpret_cast<void*>(&nativeGetBearing) },
        { "nativeResetNorth", "(J)V", reinterpret_cast<void*>(&nativeResetNorth) },
        { "nativeStartRotating", "(J)V", reinterpret_cast<void*>(&nativeStartRotating) },
        { "nativeStopRotating", "(J)V", reinterpret_cast<void*>(&nativeStopRotating) },
        { "nativeCanRotate", "(J)Z", reinterpret_cast<void*>(&nativeCanRotate) },
        { "nativeSetDebug", "(JZ)V", reinterpret_cast<void*>(&nativeSetDebug) },
        { "nativeToggleDebug", "(J)V", reinterpret_cast<void*>(&nativeToggleDebug) },
        { "nativeGetDebug", "(J)Z", reinterpret_cast<void*>(&nativeGetDebug) },
        { "nativeSetReachability", "(JZ)V", reinterpret_cast<void*>(&nativeSetReachability) }
    }};

    if (env->RegisterNatives(native_map_view_class, methods.data(), methods.size()) < 0) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_lat_class = reinterpret_cast<jclass>(env->NewGlobalRef(lon_lat_class));
    if (lon_lat_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_lat_zoom_class = reinterpret_cast<jclass>(env->NewGlobalRef(lon_lat_zoom_class));
    if (lon_lat_zoom_class == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(lon_lat_class);
        return JNI_ERR;
    }

    runtime_exception_class = reinterpret_cast<jclass>(env->NewGlobalRef(runtime_exception_class));
    if (runtime_exception_class == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(lon_lat_class);
        env->DeleteGlobalRef(lon_lat_zoom_class);
        return JNI_ERR;
    }

    null_pointer_exception_class = reinterpret_cast<jclass>(env->NewGlobalRef(null_pointer_exception_class));
    if (runtime_exception_class == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(lon_lat_class);
        env->DeleteGlobalRef(lon_lat_zoom_class);
        env->DeleteGlobalRef(runtime_exception_class);
        return JNI_ERR;
    }

    array_list_class = reinterpret_cast<jclass>(env->NewGlobalRef(array_list_class));
    if (array_list_class == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(lon_lat_class);
        env->DeleteGlobalRef(lon_lat_zoom_class);
        env->DeleteGlobalRef(runtime_exception_class);
        env->DeleteGlobalRef(null_pointer_exception_class);
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    mbgl::Log::Debug(mbgl::Event::JNI, "JNI_OnUnload");

    JNIEnv* env = nullptr;
    jint ret = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
        return;
    }

    env->DeleteGlobalRef(lon_lat_class);
    lon_lat_class = nullptr;
    lon_lat_constructor_id = nullptr;
    lon_lat_lon_id = nullptr;
    lon_lat_lat_id = nullptr;

    env->DeleteGlobalRef(lon_lat_zoom_class);
    lon_lat_zoom_class = nullptr;
    lon_lat_zoom_constructor_id = nullptr;
    lon_lat_zoom_lon_id = nullptr;
    lon_lat_zoom_lat_id = nullptr;
    lon_lat_zoom_zoom_id = nullptr;

    on_map_changed_id = nullptr;

    env->DeleteGlobalRef(runtime_exception_class);
    runtime_exception_class = nullptr;

    env->DeleteGlobalRef(null_pointer_exception_class);
    null_pointer_exception_class = nullptr;

    list_to_array_id = nullptr;

    env->DeleteGlobalRef(array_list_class);
    array_list_class = nullptr;
    array_list_constructor_id = nullptr;
    array_list_add_id = nullptr;
}

}
