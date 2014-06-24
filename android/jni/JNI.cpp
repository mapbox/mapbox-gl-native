#include <string>
#include <locale>
#include <codecvt>
#include <array>

#include <jni.h>

#include <android/native_window_jni.h>

#include "log.h"

#include "NativeMapView.hpp"

namespace llmr {
namespace android {

jmethodID on_map_changed_id = nullptr;

jclass lon_lat_class = nullptr;
jmethodID lon_lat_constructor_id = nullptr;
jfieldID lon_id = nullptr;
jfieldID lat_id = nullptr;

bool throw_error(JNIEnv* env, const char* msg) {
    jclass clazz = env->FindClass("java/lang/RuntimeException");
    if (clazz == nullptr) {
        env->ExceptionDescribe();
        return false;
    }

    if (env->ThrowNew(clazz, msg) < 0) {
        env->ExceptionDescribe();
        return false;
    }

    return true;
}

std::string std_string_from_jstring(JNIEnv* env, jstring jstr) {
    std::string str;
    const jchar* chars = env->GetStringChars(jstr, nullptr);
    if (chars == nullptr) {
        env->ExceptionDescribe();
        return str;
    }
    str = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t>().to_bytes(reinterpret_cast<const char16_t*>(chars));
    env->ReleaseStringChars(jstr, chars);
    chars = nullptr;
    return str;
}

} // namespace android
} // namespace llmr

namespace {

using namespace llmr::android;

jlong nativeCreate(JNIEnv* env, jobject obj, jstring default_style_json) {
    VERBOSE("nativeCreate");
    NativeMapView* native_map_view = new NativeMapView(env, obj, std_string_from_jstring(env, default_style_json));
    if (native_map_view == nullptr) {
        throw_error(env, "Unable to create NativeMapView.");
        return 0;
    }
    jlong map_view_ptr = reinterpret_cast<jlong>(native_map_view);
    return map_view_ptr;
}

void nativeDestroy(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeDestroy");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    delete native_map_view;
    native_map_view = nullptr;
}

void nativeInitializeContext(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeInitializeContext");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    if (!native_map_view->initializeContext()) {
        throw_error(env, "Unable to initialize GL context.");
    }
}

void nativeTerminateContext(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeTerminateContext");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->terminateContext();
}

void nativeCreateSurface(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jobject surface) {
    VERBOSE("nativeCreateSurface");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    if (!native_map_view->createSurface(ANativeWindow_fromSurface(env, surface))) {
        throw_error(env, "Unable to create GL surface.");
    }
}

void nativeDestroySurface(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeDestroySurface");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->destroySurface();
}

void nativeStart(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStart");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->start();
}

void nativeStop(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStop");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->stop();
}

void nativeUpdate(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeUpdate");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->update();
}

void nativeCleanup(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeCleanup");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->cleanup();
}

void nativeResize(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jint width, jint height) {
    VERBOSE("nativeResize");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->resize(width, height);
}

void nativeCancelTransitions(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeCancelTransitions");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->cancelTransitions();
}

jobject nativeGetLonLat(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
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

void nativeSetLonLat(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jobject lon_lat) {
    VERBOSE("nativeSetLonLat");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);

    double lon = env->GetDoubleField(lon_lat, lon_id);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }

    double lat = env->GetDoubleField(lon_lat, lat_id);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            return;
        }
    // TODO remove when fixed
    DEBUG("lon lat %f %f", lon, lat);
    native_map_view->getMap()->setLonLat(lon, lat);
}

void nativeResetPosition(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeResetPosition");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->resetPosition();
}

void nativeMoveBy(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble dx, jdouble dy) {
    VERBOSE("nativeMoveBy");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->moveBy(dx, dy);
}

jdouble nativeGetZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetZoom");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getZoom();
}

void nativeSetZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble zoom) {
    VERBOSE("nativeSetZoom");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setZoom(zoom);
}

void nativeScaleBy(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble ds, jdouble cx, jdouble cy, jdouble duration) {
    VERBOSE("nativeScaleBy");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->scaleBy(ds, cx, cy, duration);
}

jdouble nativeGetAngle(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetAngle");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getAngle();
}

void nativeResetNorth(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeResetNorth");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->resetNorth();
}

void nativeSetAngle(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble angle) {
    VERBOSE("nativeSetAngle");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setAngle(angle);
}

jboolean nativeGetDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetDebug");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getDebug();
}

void nativeSetDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jboolean debug) {
    VERBOSE("nativeSetDebug");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setDebug(debug);
}

void nativeToggleDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeToggleDebug");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->toggleDebug();
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

    lon_lat_class = env->FindClass("com/mapbox/mapboxgl/LonLat");
    if (lon_lat_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_lat_constructor_id = env->GetMethodID(lon_lat_class, "<init>", "(DD)V");
    if (lon_lat_constructor_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lon_id = env->GetFieldID(lon_lat_class, "lon", "D");
    if (lon_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    lat_id = env->GetFieldID(lon_lat_class, "lat", "D");
    if (lat_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    jclass native_map_view_class = env->FindClass("com/mapbox/mapboxgl/NativeMapView");
    if (native_map_view_class == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    on_map_changed_id = env->GetMethodID(native_map_view_class, "onMapChanged", "()V");
    if (on_map_changed_id == nullptr) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }

    std::array<JNINativeMethod, 26> methods = {{ // TODO shouldn't need double braces.. missing feature in clang?
        // http://social.msdn.microsoft.com/forums/vstudio/en-US/e5ad8fa5-c9e8-4328-a7fa-af7a47ce2492/initialising-a-stdarray-of-structs
        { "nativeCreate", "(Ljava/lang/String;)J", reinterpret_cast<void*>(&nativeCreate) },
        { "nativeDestroy", "(J)V", reinterpret_cast<void*>(&nativeDestroy) },
        { "nativeInitializeContext", "(J)V", reinterpret_cast<void*>(&nativeInitializeContext) },
        { "nativeTerminateContext", "(J)V", reinterpret_cast<void*>(&nativeTerminateContext) },
        { "nativeCreateSurface", "(JLandroid/view/Surface;)V", reinterpret_cast<void*>(&nativeCreateSurface) },
        { "nativeDestroySurface", "(J)V", reinterpret_cast<void*>(&nativeDestroySurface) },
        { "nativeStart", "(J)V", reinterpret_cast<void*>(&nativeStart) },
        { "nativeStop", "(J)V", reinterpret_cast<void*>(&nativeStop) },
        { "nativeUpdate", "(J)V", reinterpret_cast<void*>(&nativeUpdate) },
        { "nativeCleanup", "(J)V", reinterpret_cast<void*>(&nativeCleanup) },
        { "nativeResize", "(JII)V", reinterpret_cast<void*>(&nativeResize) },
        { "nativeCancelTransitions", "(J)V", reinterpret_cast<void*>(&nativeCancelTransitions) },
        { "nativeGetLonLat", "(J)Lcom/mapbox/mapboxgl/LonLat;", reinterpret_cast<void*>(&nativeGetLonLat) },
        { "nativeSetLonLat", "(JLcom/mapbox/mapboxgl/LonLat;)V", reinterpret_cast<void*>(&nativeSetLonLat) },
        { "nativeResetPosition", "(J)V", reinterpret_cast<void*>(&nativeResetPosition) },
        { "nativeMoveBy", "(JDDD)V", reinterpret_cast<void*>(&nativeMoveBy) },
        { "nativeGetZoom", "(J)D", reinterpret_cast<void*>(&nativeGetZoom) },
        { "nativeSetZoom", "(JD)V", reinterpret_cast<void*>(&nativeSetZoom) },
        { "nativeScaleBy", "(JDDDD)V", reinterpret_cast<void*>(&nativeScaleBy) },
        { "nativeGetAngle", "(J)D", reinterpret_cast<void*>(&nativeGetAngle) },
        { "nativeSetAngle", "(JD)V", reinterpret_cast<void*>(&nativeSetAngle) },
        { "nativeResetNorth", "(J)V", reinterpret_cast<void*>(&nativeResetNorth) },
        { "nativeGetDebug", "(J)Z", reinterpret_cast<void*>(&nativeGetDebug) },
        { "nativeSetDebug", "(JZ)V", reinterpret_cast<void*>(&nativeSetDebug) },
        { "nativeToggleDebug", "(J)V", reinterpret_cast<void*>(&nativeToggleDebug) }}
    };
    jint ret = env->RegisterNatives(native_map_view_class, methods.data(), methods.size());
    DEBUG("RegisterNatives returned %d", ret);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }
    /* Need to wait for https://code.google.com/p/android/issues/detail?id=72293
    if (env->RegisterNatives(clazz, methods.data(), methods.size()) < 0) {
        env->ExceptionDescribe();
        return JNI_ERR;
    }*/

    lon_lat_class = reinterpret_cast<jclass>(env->NewGlobalRef(lon_lat_class));
    if (lon_lat_class == nullptr) {
        env->ExceptionDescribe();
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
    lon_id = nullptr;
    lat_id = nullptr;

    on_map_changed_id = nullptr;
}

} // extern "C"
