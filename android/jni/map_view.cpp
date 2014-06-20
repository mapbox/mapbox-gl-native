#include <stdio.h>

#include <string>

#include <jni.h>

#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <EGL/egl.h>

#include <GLES2/gl2.h>

#include <llmr/llmr.hpp>
#include <llmr/platform/platform.hpp>

#define TAG "map_view.cpp"

#define INFO(fmt, args...) __android_log_print(ANDROID_LOG_INFO, TAG, fmt, ##args)
#define ERROR(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)
#define ASSERT(cond, ...) ((cond)?(void)0:__android_log_assert("##cond", TAG, ""))
#define ASSERTMSG(cond, fmt, args...) ((cond)?(void)0:__android_log_assert("##cond", TAG, fmt, ##args))

class NativeMapView : public llmr::View {
private:
    ANativeWindow* window;

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;

    bool initialized = false;

    std::string default_style_json;

public:
    NativeMapView(ANativeWindow* window, std::string default_style_json) : window(window),
                                                                           default_style_json(default_style_json) {
        INFO("NativeMapView constructor");
        ASSERT(window != nullptr);
    }

    ~NativeMapView() {
        INFO("NativeMapView destructor");
        destroy();

        if (map != nullptr) {
            delete map;
            map = nullptr;
        }

        ANativeWindow_release(window);
        window = nullptr;
    }

    bool initialize() {
        INFO("NativeMapView initialize");

        llmr::Map* map = new llmr::Map(*this);
        if (map == nullptr) {
            ERROR("llmr::Map allocation failed");
            return false;
        }
        if (!initialized) {
            ERROR("NativeMapView context initialization failed");
            return false;
        }

        return true;
    }

    void initialize(llmr::Map* map) override {
        INFO("NativeMapView initialize context");

        ASSERT(!initialized);

        freopen("/sdcard/stdout.txt", "w", stdout); // NOTE: can't use <cstdio> till NDK fix the stdout macro bug
        freopen("/sdcard/stderr.txt", "w", stderr);

        View::initialize(map);

        ASSERT(display == EGL_NO_DISPLAY);
        ASSERT(surface == EGL_NO_SURFACE);
        ASSERT(context == EGL_NO_CONTEXT);

        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            ERROR("eglGetDisplay() returned error %d", eglGetError());
            destroy();
            return;
        }

        EGLint major, minor;
        if (!eglInitialize(display, &major, &minor)) {
            ERROR("eglInitialize() returned error %d", eglGetError());
            destroy();
            return;
        }
        if ((major <= 1) && (minor < 3)) {
            ERROR("EGL version is too low, need 1.3, got %d.%d", major, minor);
            destroy();
            return;
        }
        INFO("EGL version is %d.%d", major, minor);

        const EGLint config_attribs[] = {
            EGL_CONFIG_CAVEAT, EGL_NONE,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
        };
        EGLConfig config;
        EGLint num_configs;
        if (!eglChooseConfig(display, config_attribs, &config, 1, &num_configs)) {
            ERROR("eglChooseConfig() returned error %d", eglGetError());
            destroy();
            return;
        }
        if (num_configs < 1) {
            ERROR("eglChooseConfig() returned no configs");
            destroy();
            return;
        }

        EGLint format;
        if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
            ERROR("eglGetConfigAttrib() returned error %d", eglGetError());
            destroy();
            return;
        }
        INFO("Chosen window format is %d", format);

        ANativeWindow_setBuffersGeometry(window, 0, 0, format);

        const EGLint surface_attribs[] = {
            EGL_NONE
        };
        surface = eglCreateWindowSurface(display, config, window, surface_attribs);
        if (surface == EGL_NO_SURFACE) {
            ERROR("eglCreateWindowSurface() returned error %d", eglGetError());
            destroy();
            return;
        }

        EGLint width, height;
        if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
            !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
            ERROR("eglQuerySurface() returned error %d", eglGetError());
            destroy();
            return;
        }

        const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
        if (context == EGL_NO_CONTEXT) {
            ERROR("eglCreateContext() returned error %d", eglGetError());
            destroy();
            return;
        }

        if (!eglMakeCurrent(display, surface, surface, context)) {
            ERROR("eglMakeCurrent() returned error %d", eglGetError());
            destroy();
            return;
        }

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
            ERROR("eglMakeCurrent(EGL_NO_CONTEXT) returned error %d", eglGetError());
            destroy();
            return;
        }

        resize(width, height);

        map->setDebug(true);
        map->setStyleJSON(default_style_json);
        map->setLonLatZoom(0, 0, 0);
        map->start();

        INFO("Context initialized");

        initialized = true;
    }

    void resize(int width, int height) {
        map->resize(width, height);
    }

    void redraw() {
        map->update();
    }

    void stop() {
        map->cleanup();
    }

    void swap() override {
        if (map->needsSwap()) {
            eglSwapBuffers(display, surface);
            map->swapped();
        }
    }

    void make_active() override {
        // TODO how do we undo this? If thread is different from init thread?
        eglMakeCurrent(display, surface, surface, context);
    }

    void pause() {
        map->stop();
    }

    void resume() {
        map->start();
    }

private:
    void destroy() {
        INFO("NativeMapView destroy");

        if (display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

            if (context != EGL_NO_CONTEXT) {
                eglDestroyContext(display, context);
            }

            if (surface != EGL_NO_SURFACE) {
                eglDestroySurface(display, surface);
            }

            eglTerminate(display);
        }

        context = EGL_NO_CONTEXT;
        surface = EGL_NO_SURFACE;
        display = EGL_NO_DISPLAY;
    }
};

void llmr::platform::notify_map_change() {
    INFO("notify_map_change() called");
    // TODO is only one instance of the map allowed?
}

std::string std_string_from_jstring(JNIEnv* env, jstring jstr) {
    std::string str;
    const char* chars = env->GetStringUTFChars(jstr, nullptr);
    str = chars;
    env->ReleaseStringUTFChars(jstr, chars);
    chars = nullptr;
    return str;
}

extern "C" JNIEXPORT jlong JNICALL Java_com_mapbox_mapboxgl_MapView_nativeSurfaceCreated(JNIEnv* env, jclass clazz, jobject surface, jstring default_style_json) {
    INFO("nativeSurfaceCreated");
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
    INFO("nativeSurfaceDestroyed");
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    delete map_view;
    map_view = nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeSurfaceChanged(JNIEnv* env, jclass clazz, jlong map_view_ptr, jint format, jint width, jint height) {
    INFO("nativeSurfaceChanged");
    INFO("Surface changed, format %d, width %d, height %d", format, width, height);
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    map_view->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeSurfaceRedrawNeeded(JNIEnv* env, jclass clazz, jlong map_view_ptr) {
    INFO("nativeSurfaceRedrawNeeded");
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    map_view->redraw();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeOnPause(JNIEnv* env, jclass clazz, jlong map_view_ptr) {
    INFO("nativeOnPause");
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    map_view->pause();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeOnResume(JNIEnv* env, jclass clazz, jlong map_view_ptr) {
    INFO("nativeOnResume");
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    map_view->resume();
}

extern "C" JNIEXPORT void JNICALL Java_com_mapbox_mapboxgl_MapView_nativeOnStop(JNIEnv* env, jclass clazz, jlong map_view_ptr) {
    INFO("nativeOnStop");
    ASSERT(map_view_ptr != 0);
    NativeMapView* map_view = reinterpret_cast<NativeMapView*>(map_view_ptr);
    map_view->stop();
}
