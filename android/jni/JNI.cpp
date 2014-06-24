#include <string>

#include <jni.h>

#include <android/native_window_jni.h>

#include "log.h"

#include "NativeMapView.hpp"
// TODO mod utf 8 everywhere
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
    const char* chars = env->GetStringUTFChars(jstr, nullptr);
    if (chars == nullptr) {
    	env->ExceptionDescribe();
    	return str;
    }
    str = chars;
    env->ReleaseStringUTFChars(jstr, chars);
    chars = nullptr;
    return str;
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
	return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
	// no-op
}

extern "C" JNIEXPORT jlong JNICALL Java_com_mapbox_mapboxgl_MapView_nativeCreate(JNIEnv* env, jobject obj, jstring default_style_json) {
    VERBOSE("nativeCreate");
    NativeMapView* native_map_view = new NativeMapView(env, obj, std_string_from_jstring(env, default_style_json));
    if (native_map_view == nullptr) {
    	throw_error(env, "Unable to create NativeMapView.");
    	return 0;
    }
    jlong map_view_ptr = reinterpret_cast<jlong>(native_map_view);
    return map_view_ptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeDestroy(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeDestroy");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    delete native_map_view;
    native_map_view = nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeInitializeContext(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeInitializeContext");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    if (!native_map_view->initializeContext()) {
    	throw_error(env, "Unable to initialize GL context.");
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeTerminateContext(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeTerminateContext");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->terminateContext();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeCreateSurface(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jobject surface) {
    VERBOSE("nativeCreateSurface");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    if (!native_map_view->createSurface(ANativeWindow_fromSurface(env, surface))) {
    	throw_error(env, "Unable to create GL surface.");
    }
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeDestroySurface(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeDestroySurface");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->destroySurface();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeStart(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStart");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->start();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeStop(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeStop");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->stop();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeUpdate(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeUpdate");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->update();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeCleanup(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeCleanup");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->cleanup();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeResize(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jint width, jint height) {
    VERBOSE("nativeResize");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeCancelTransitions(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeCancelTransitions");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->cancelTransitions();
}

extern "C" JNIEXPORT jdouble JNICALL Java_com_mapbox_mapboxgl_MapView_nativeGetLon(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetLon");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    double lon, lat;
    native_map_view->getMap()->getLonLat(lon, lat);
    return lon;
}

extern "C" JNIEXPORT jdouble JNICALL Java_com_mapbox_mapboxgl_MapView_nativeGetLat(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetLat");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    double lon, lat;
    native_map_view->getMap()->getLonLat(lon, lat);
    return lat;
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeSetLonLat(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble lon, jdouble lat) {
    VERBOSE("nativeSetLonLat");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setLonLat(lon, lat);
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeResetPosition(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeResetPosition");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->resetPosition();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeMoveBy(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble dx, jdouble dy) {
    VERBOSE("nativeMoveBy");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->moveBy(dx, dy);
}

extern "C" JNIEXPORT jdouble JNICALL Java_com_mapbox_mapboxgl_MapView_nativeGetZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetZoom");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getZoom();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeSetZoom(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble zoom) {
    VERBOSE("nativeSetZoom");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setZoom(zoom);
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeScaleBy(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble ds, jdouble cx, jdouble cy, jdouble duration) {
    VERBOSE("nativeScaleBy");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->scaleBy(ds, cx, cy, duration);
}

extern "C" JNIEXPORT jdouble JNICALL Java_com_mapbox_mapboxgl_MapView_nativeGetAngle(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetAngle");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getAngle();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeSetAngle(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jdouble angle) {
    VERBOSE("nativeSetAngle");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setAngle(angle);
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_mapbox_mapboxgl_MapView_nativeGetDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeGetDebug");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    return native_map_view->getMap()->getDebug();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeSetDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr, jboolean debug) {
    VERBOSE("nativeSetDebug");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->setDebug(debug);
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeToggleDebug(JNIEnv* env, jobject obj, jlong native_map_view_ptr) {
    VERBOSE("nativeToggleDebug");
    ASSERT(native_map_view_ptr != 0);
    NativeMapView* native_map_view = reinterpret_cast<NativeMapView*>(native_map_view_ptr);
    native_map_view->getMap()->toggleDebug();
}
