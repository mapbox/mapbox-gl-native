// Work around error in Android's Bionic stdint.h
// Should be fixed in next NDK
// Seems to be specific to C++11 (older versions needed these macros)
#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#define __STDC_FORMAT_MACROS
#include <cstdint>
#include <cinttypes>

#include <string>
#include <locale>
#include <codecvt>
#include <array>
#include <set>

#include <jni.h>

#include <android/native_window_jni.h>

#include "log.h"

#include "NativeMapView.hpp"

namespace llmr {
namespace android {

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

jmethodID set_to_array_id = nullptr;

jclass tree_set_class = nullptr;
jmethodID tree_set_constructor_id = nullptr;
jmethodID tree_set_add_id = nullptr;

bool throw_error(JNIEnv* env, const char* msg) {
    if (env->ThrowNew(runtime_exception_class, msg) < 0) {
        env->ExceptionDescribe();
        return false;
    }

    return true;
}

std::string std_string_from_jstring(JNIEnv* env, jstring jstr) {
    std::string str;
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

std::set<std::string> std_set_string_from_jobject(JNIEnv* env, jobject jset) {
    std::set<std::string> set;

    jobjectArray array = reinterpret_cast<jobjectArray>(env->CallObjectMethod(jset, set_to_array_id));
    if (env->ExceptionCheck() || (array == nullptr)) {
        env->ExceptionDescribe();
        return set;
    }

    jsize len = env->GetArrayLength(array);
    if (len < 0) {
        env->ExceptionDescribe();
        return set;
    }

    for (jsize i = 0; i < len; i++) {
        jstring jstr = reinterpret_cast<jstring>(env->GetObjectArrayElement(array, i));
        if (jstr == nullptr) {
            env->ExceptionDescribe();
            return set;
        }

        set.insert(std_string_from_jstring(env, jstr));
    }

    return set;
}

jobject std_set_string_to_jobject(JNIEnv* env, std::set<std::string> set)  {
    jobject jset = env->NewObject(tree_set_class, tree_set_constructor_id);
    if (jset == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    for (std::string str : set) {
        env->CallBooleanMethod(jset, tree_set_add_id, std_string_to_jstring(env, str));
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return nullptr;
        }
    }

    return jset;
}

} // namespace android
} // namespace llmr

namespace {

using namespace llmr::android;

// TODO: wrap C++ exceptions?
// TODO: wrap other sorts of exceptions? eg coffee catch

jlong JNICALL nativeCreate(JNIEnv* env, jobject obj, jstring default_style_json) {
    VERBOSE("nativeCreate");
    NativeMapView* native_map_view = new NativeMapView(env, obj, std_string_from_jstring(env, default_style_json));
    if (native_map_view == nullptr) {
        throw_error(env, "Unable to create NativeMapView.");
        return 0;
    }
    jlong map_view_ptr = reinterpret_cast<jlong>(native_map_view);
    return map_view_ptr;
}

void JNICALL nativeDestroy(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeDestroy");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    delete native_map_view;
    native_map_view = nullptr;
}

void JNICALL nativeInitializeDisplay(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeInitializeDisplay");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    if (!native_map_view->initializeDisplay()) {
        throw_error(env, "Unable to initialize GL display.");
    }
}

void JNICALL nativeTerminateDisplay(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeTerminateDisplay");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->terminateDisplay();
}

void JNICALL nativeInitializeContext(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeInitializeContext");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    if (!native_map_view->initializeContext()) {
        throw_error(env, "Unable to initialize GL context.");
    }
}

void JNICALL nativeTerminateContext(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeTerminateContext");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->terminateContext();
}

void JNICALL nativeCreateSurface(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jobject surface) {
    VERBOSE("nativeCreateSurface");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    if (!native_map_view->createSurface(ANativeWindow_fromSurface(env, surface))) {
        throw_error(env, "Unable to create GL surface.");
    }
}

void JNICALL nativeDestroySurface(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeDestroySurface");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->destroySurface();
}

void JNICALL nativeStart(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStart");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->start();
}

void JNICALL nativeStop(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStop");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->stop();
}

void JNICALL nativeRerender(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeRerender");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->rerender();
}

void JNICALL nativeUpdate(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeUpdate");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->update();
}

void JNICALL nativeCleanup(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeCleanup");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->cleanup();
}

void JNICALL nativeAddDefaultSource(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeAddDefaultSource");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->addDefaultSource();
}

void JNICALL nativeRemoveDefaultSource(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeRemoveDefaultSource");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->removeDefaultSource();
}

void JNICALL nativeAddSource(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jstring name, jstring url) {
    VERBOSE("nativeAddSource");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->addSource(std_string_from_jstring(env, name), std_string_from_jstring(env, url));
}

void JNICALL nativeRemoveSource(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jstring name) {
    VERBOSE("nativeRemoveSource");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->removeSource(std_string_from_jstring(env, name));
}

void JNICALL nativeResize(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jint width, jint height, jfloat ratio) {
    VERBOSE("nativeResize");
    ASSERT(native_map_view_ptr != 0);
    ASSERT(width >= 0);
    ASSERT(height >= 0);
    ASSERT(width <= UINT16_MAX);
    ASSERT(height <= UINT16_MAX);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->resize(width, height, ratio);
}

void JNICALL nativeResize(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jint width, jint height, jfloat ratio, jint fb_width, jint fb_height) {
    VERBOSE("nativeResize");
    ASSERT(native_map_view_ptr != 0);
    ASSERT(width >= 0);
    ASSERT(height >= 0);
    ASSERT(width <= UINT16_MAX);
    ASSERT(height <= UINT16_MAX);
    ASSERT(fb_width >= 0);
    ASSERT(fb_height >= 0);
    ASSERT(fb_width <= UINT16_MAX);
    ASSERT(fb_height <= UINT16_MAX);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->resize(width, height, ratio, fb_width, fb_height);
}

void JNICALL nativeSetAppliedClasses(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jobject applied_classes) {
    VERBOSE("nativeSetAppliedClasses");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setAppliedClasses(std_set_string_from_jobject(env, applied_classes));
}

jobject JNICALL nativeGetAppliedClasses(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetAppliedClasses");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return std_set_string_to_jobject(env, native_map_view->getMap()->getAppliedClasses());
}

void JNICALL nativeSetDefaultTransitionDuration(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jlong duration_milliseconds) {
    VERBOSE("nativeSetDefaultTransitionDuration");
    ASSERT(native_map_view_ptr != 0);
    ASSERT(duration_milliseconds >= 0);
    ASSERT(duration_milliseconds <= UINT64_MAX);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setDefaultTransitionDuration(duration_milliseconds);
}

void JNICALL nativeSetStyleJSON(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jstring new_style_json) {
    VERBOSE("nativeSetStyleJSON");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setStyleJSON(std_string_from_jstring(env, new_style_json));
}

jstring JNICALL nativeGetStyleJSON(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetStyleJSON");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return std_string_to_jstring(env, native_map_view->getMap()->getStyleJSON());
}

void JNICALL nativeCancelTransitions(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeCancelTransitions");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->cancelTransitions();
}

void JNICALL nativeMoveBy(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble dx, jdouble dy, jdouble duration) {
    VERBOSE("nativeMoveBy");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->moveBy(dx, dy, duration);
}

void JNICALL nativeSetLonLat(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jobject lon_lat, jdouble duration) {
    VERBOSE("nativeSetLonLat");
    ASSERT(native_map_view_ptr != 0);
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

    // TODO remove when fixed
    DEBUG("lon lat %f %f", lon, lat);
    native_map_view->getMap()->setLonLat(lon, lat, duration);
}

jobject JNICALL nativeGetLonLat(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetLonLat");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    double lon, lat;
    native_map_view->getMap()->getLonLat(lon, lat);

    jobject ret = env->NewObject(lon_lat_class, lon_lat_constructor_id, lon, lat);
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}

void JNICALL nativeStartPanning(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStartPanning");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->startPanning();
}

void JNICALL nativeStopPanning(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStopPanning");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->stopPanning();
}

void JNICALL nativeResetPosition(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeResetPosition");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->resetPosition();
}

void JNICALL nativeScaleBy(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble ds, jdouble cx, jdouble cy, jdouble duration) {
    VERBOSE("nativeScaleBy");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->scaleBy(ds, cx, cy, duration);
}

void JNICALL nativeSetScale(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble scale, jdouble cx, jdouble cy, jdouble duration) {
    VERBOSE("nativeSetScale");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setScale(scale, cx, cy, duration);
}

jdouble JNICALL nativeGetScale(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetScale");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getScale();
}

void JNICALL nativeSetZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble zoom, jdouble duration) {
    VERBOSE("nativeSetZoom");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setZoom(zoom, duration);
}

jdouble JNICALL nativeGetZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetZoom");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getZoom();
}

void JNICALL nativeSetLonLatZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jobject lon_lat_zoom, jdouble duration) {
    VERBOSE("nativeSetLonLatZoom");
    ASSERT(native_map_view_ptr != 0);
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

    native_map_view->getMap()->setLonLatZoom(lon, lat, zoom, duration);
}

jobject JNICALL nativeGetLonLatZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetLonLatZoom");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    double lon, lat, zoom;
    native_map_view->getMap()->getLonLatZoom(lon, lat, zoom);

    jobject ret = env->NewObject(lon_lat_zoom_class, lon_lat_zoom_constructor_id, lon, lat, zoom);
    if (ret == nullptr) {
        env->ExceptionDescribe();
        return nullptr;
    }

    return ret;
}

void JNICALL nativeResetZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeResetZoom");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->resetZoom();
}

void JNICALL nativeStartScaling(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStartScaling");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->startScaling();
}

void JNICALL nativeStopScaling(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStopScaling");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->stopScaling();
}

jdouble JNICALL nativeGetMinZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetMinZoom");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getMinZoom();
}

jdouble JNICALL nativeGetMaxZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetMaxZoom");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getMaxZoom();
}

void JNICALL nativeRotateBy(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble sx, jdouble sy, jdouble ex, jdouble ey, jdouble duration) {
    VERBOSE("nativeRotateBy");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->rotateBy(sx, sy, ex, ey, duration);
}

void JNICALL nativeSetAngle(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble angle, jdouble duration) {
    VERBOSE("nativeSetAngle");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setAngle(angle, duration);
}

void JNICALL nativeSetAngle(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble angle, jdouble cx, jdouble cy) {
    VERBOSE("nativeSetAngle");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setAngle(angle, cx, cy);
}

jdouble JNICALL nativeGetAngle(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetAngle");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getAngle();
}

void JNICALL nativeResetNorth(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeResetNorth");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->resetNorth();
}

void JNICALL nativeStartRotating(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStartRotating");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->startRotating();
}

void JNICALL nativeStopRotating(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStopRotating");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->stopRotating();
}

jboolean JNICALL nativeCanRotate(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeCanRotate");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->canRotate();
}

void JNICALL nativeSetDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jboolean debug) {
    VERBOSE("nativeSetDebug");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setDebug(debug);
}

void JNICALL nativeToggleDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeToggleDebug");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->toggleDebug();
}

jboolean JNICALL nativeGetDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetDebug");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getDebug();
}

} // namespace

extern "C" {

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    VERBOSE("JNI_OnLoad");

    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        env->ExceptionDescribe();
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

    jclass set_class = env->FindClass("java/util/Set");
    if (set_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    set_to_array_id = env->GetMethodID(set_class, "toArray", "()[Ljava/lang/Object;");
    if (set_to_array_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    tree_set_class = env->FindClass("java/util/TreeSet");
    if (tree_set_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    tree_set_constructor_id = env->GetMethodID(tree_set_class, "<init>", "()V");
    if (tree_set_constructor_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    tree_set_add_id = env->GetMethodID(tree_set_class, "add", "(Ljava/lang/Object;)Z");
    if (tree_set_add_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    std::array<JNINativeMethod, 54> methods = {{ // Can remove the extra brace in C++14
        { "nativeCreate", "(Ljava/lang/String;)J", reinterpret_cast<void*>(&nativeCreate) },
        { "nativeDestroy", "(J)V", reinterpret_cast<void*>(&nativeDestroy) },
        { "nativeInitializeDisplay", "(J)V", reinterpret_cast<void*>(&nativeInitializeDisplay) },
        { "nativeTerminateDisplay", "(J)V", reinterpret_cast<void*>(&nativeTerminateDisplay) },
        { "nativeInitializeContext", "(J)V", reinterpret_cast<void*>(&nativeInitializeContext) },
        { "nativeTerminateContext", "(J)V", reinterpret_cast<void*>(&nativeTerminateContext) },
        { "nativeCreateSurface", "(JLandroid/view/Surface;)V", reinterpret_cast<void*>(&nativeCreateSurface) },
        { "nativeDestroySurface", "(J)V", reinterpret_cast<void*>(&nativeDestroySurface) },
        { "nativeStart", "(J)V", reinterpret_cast<void*>(&nativeStart) },
        { "nativeStop", "(J)V", reinterpret_cast<void*>(&nativeStop) },
        { "nativeRerender", "(J)V", reinterpret_cast<void*>(&nativeRerender) },
        { "nativeUpdate", "(J)V", reinterpret_cast<void*>(&nativeUpdate) },
        { "nativeCleanup", "(J)V", reinterpret_cast<void*>(&nativeCleanup) },
        { "nativeAddDefaultSource", "(J)V", reinterpret_cast<void*>(&nativeAddDefaultSource) },
        { "nativeRemoveDefaultSource", "(J)V", reinterpret_cast<void*>(&nativeRemoveDefaultSource) },
        { "nativeAddSource", "(JLjava/lang/String;Ljava/lang/String;)V", reinterpret_cast<void*>(&nativeAddSource) },
        { "nativeRemoveSource", "(JLjava/lang/String;)V", reinterpret_cast<void*>(&nativeRemoveSource) },
        { "nativeResize", "(JIIF)V", reinterpret_cast<void*>(static_cast<void JNICALL(*)(JNIEnv*,jobject,jlong,jint,jint,jfloat)>(&nativeResize)) },
        { "nativeResize", "(JIIFII)V", reinterpret_cast<void*>(static_cast<void JNICALL(*)(JNIEnv*,jobject,jlong,jint,jint,jfloat,jint,jint)>(&nativeResize)) },
        { "nativeSetAppliedClasses", "(JLjava/util/Set;)V", reinterpret_cast<void*>(&nativeSetAppliedClasses) },
        { "nativeGetAppliedClasses", "(J)Ljava/util/Set;", reinterpret_cast<void*>(&nativeGetAppliedClasses) },
        { "nativeSetDefaultTransitionDuration", "(JJ)V", reinterpret_cast<void*>(&nativeSetDefaultTransitionDuration) },
        { "nativeSetStyleJSON", "(JLjava/lang/String;)V", reinterpret_cast<void*>(&nativeSetStyleJSON) },
        { "nativeGetStyleJSON", "(J)Ljava/lang/String;", reinterpret_cast<void*>(&nativeGetStyleJSON) },
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
        { "nativeSetAngle", "(JDD)V", reinterpret_cast<void*>(static_cast<void JNICALL(*)(JNIEnv*,jobject,jlong,jdouble,jdouble)>(&nativeSetAngle)) },
        { "nativeSetAngle", "(JDDD)V", reinterpret_cast<void*>(static_cast<void JNICALL(*)(JNIEnv*,jobject,jlong,jdouble,jdouble,jdouble)>(&nativeSetAngle)) },
        { "nativeGetAngle", "(J)D", reinterpret_cast<void*>(&nativeGetAngle) },
        { "nativeResetNorth", "(J)V", reinterpret_cast<void*>(&nativeResetNorth) },
        { "nativeStartRotating", "(J)V", reinterpret_cast<void*>(&nativeStartRotating) },
        { "nativeStopRotating", "(J)V", reinterpret_cast<void*>(&nativeStopRotating) },
        { "nativeCanRotate", "(J)Z", reinterpret_cast<void*>(&nativeCanRotate) },
        { "nativeSetDebug", "(JZ)V", reinterpret_cast<void*>(&nativeSetDebug) },
        { "nativeToggleDebug", "(J)V", reinterpret_cast<void*>(&nativeToggleDebug) },
        { "nativeGetDebug", "(J)Z", reinterpret_cast<void*>(&nativeGetDebug) }
    }};
    jint ret = env->RegisterNatives(native_map_view_class, methods.data(), methods.size());
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

    tree_set_class = reinterpret_cast<jclass>(env->NewGlobalRef(tree_set_class));
    if (tree_set_class == nullptr) {
        env->ExceptionDescribe();
        env->DeleteGlobalRef(lon_lat_class);
        env->DeleteGlobalRef(lon_lat_zoom_class);
        env->DeleteGlobalRef(runtime_exception_class);
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    VERBOSE("JNI_OnUnload");

    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        env->ExceptionDescribe();
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

    set_to_array_id = nullptr;

    env->DeleteGlobalRef(tree_set_class);
    tree_set_class = nullptr;
    tree_set_constructor_id = nullptr;
    tree_set_add_id = nullptr;
}

} // extern "C"
