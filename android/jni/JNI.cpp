#include <string>

#include <jni.h>

#include <android/native_window_jni.h>

#include "log.h"

#include "NativeMapView.hpp"

std::string std_string_from_jstring(JNIEnv* env, jstring jstr) {
    std::string str;
    const char* chars = env->GetStringUTFChars(jstr, nullptr);
    str = chars;
    env->ReleaseStringUTFChars(jstr, chars);
    chars = nullptr;
    return str;
}

extern "C" JNIEXPORT jlong JNICALL Java_com_mapbox_mapboxgl_MapView_nativeSurfaceCreated(JNIEnv* env, jclass clazz, jobject surface, jstring default_style_json) {
	VERBOSE("nativeSurfaceCreated");
    NativeMapView* map_view = new NativeMapView(ANativeWindow_fromSurface(env, surface), std_string_from_jstring(env, default_style_json));
    if (!map_view->initialize()) {
        ERROR("NativeMapView initialization failed");
        delete map_view;
        map_view = nullptr;
        return 0;
    }
    jlong map_view_ptr = reinterpret_cast<jlong>(map_view);
    return map_view_ptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeSurfaceDestroyed(JNIEnv* env, jclass clazz, jlong map_view_ptr) {
	VERBOSE("nativeSurfaceDestroyed");
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    delete map_view;
    map_view = nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeSurfaceChanged(JNIEnv* env, jclass clazz, jlong map_view_ptr, jint format, jint width, jint height) {
	VERBOSE("nativeSurfaceChanged");
    INFO("Surface changed, format %d, width %d, height %d", format, width, height);
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    map_view->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeSurfaceRedrawNeeded(JNIEnv* env, jclass clazz, jlong map_view_ptr) {
	VERBOSE("nativeSurfaceRedrawNeeded");
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    map_view->redraw();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeOnPause(JNIEnv* env, jclass clazz, jlong map_view_ptr) {
	VERBOSE("nativeOnPause");
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    map_view->pause();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeOnResume(JNIEnv* env, jclass clazz, jlong map_view_ptr) {
	VERBOSE("nativeOnResume");
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    map_view->resume();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeOnStop(JNIEnv* env, jclass clazz, jlong map_view_ptr) {
	VERBOSE("nativeOnStop");
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    map_view->stop();
}
