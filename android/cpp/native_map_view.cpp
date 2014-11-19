#include <cstdlib>
#include <memory>

#include <boost/make_unique.hpp>

#include <sys/system_properties.h>

#include <GLES2/gl2.h>

#include <mbgl/android/native_map_view.hpp>
#include <mbgl/android/jni.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/event.hpp>
#include <mbgl/platform/log.hpp>

namespace mbgl {
namespace android {

void log_egl_string(EGLDisplay display, EGLint name, const char* label) {
    const char* str = eglQueryString(display, name);
    if (str == nullptr) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglQueryString(%d) returned error %d", name, eglGetError());
    } else {
        mbgl::Log::Info(mbgl::Event::OpenGL, "EGL %s: %s", label, str);
    }
}

void log_gl_string(GLenum name, const char* label) {
    const GLubyte* str = glGetString(name);
    if (str == nullptr) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "glGetString(%d) returned error %d", name, glGetError());
    } else {
        mbgl::Log::Info(mbgl::Event::OpenGL, "GL %s: %s", label, str);
    }
}


void MBGLView::make_active() {
    mbgl::Log::Debug(mbgl::Event::Android, "MBGLView::make_active");
    if ((nativeView.display != EGL_NO_DISPLAY)
            && (nativeView.surface != EGL_NO_SURFACE)
            && (nativeView.context != EGL_NO_CONTEXT)) {
        if (!eglMakeCurrent(nativeView.display, nativeView.surface,
                nativeView.surface, nativeView.context)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent() returned error %d", eglGetError());
        }
    } else {
        mbgl::Log::Info(mbgl::Event::Android, "Not activating as we are not ready");
    }
}

void MBGLView::make_inactive() {
    mbgl::Log::Debug(mbgl::Event::Android, "MBGLView::make_inactive");
    if (!eglMakeCurrent(nativeView.display, EGL_NO_SURFACE, EGL_NO_SURFACE,
            EGL_NO_CONTEXT)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                eglGetError());
    }
}

void MBGLView::swap() {
    mbgl::Log::Debug(mbgl::Event::Android, "MBGLView::swap");

    if (map->needsSwap() && (nativeView.display != EGL_NO_DISPLAY)
            && (nativeView.surface != EGL_NO_SURFACE)) {
        if (!eglSwapBuffers(nativeView.display, nativeView.surface)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglSwapBuffers() returned error %d", eglGetError());
        }
        map->swapped();
    } else {
        mbgl::Log::Info(mbgl::Event::Android, "Not swapping as we are not ready");
    }
}

void MBGLView::notify() {
    mbgl::Log::Debug(mbgl::Event::Android, "MBGLView::notify()");
    // noop
}

void MBGLView::notify_map_change(mbgl::MapChange /* change */, mbgl::timestamp /* delay */) {
    mbgl::Log::Debug(mbgl::Event::Android, "MBGLView::notify_map_change()");
    nativeView.notifyMapChange();
}

NativeMapView::NativeMapView(JNIEnv* env, jobject obj_) : view(*this), map(view) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::NativeMapView");

    assert(env != nullptr);
    assert(obj_ != nullptr);

    if (env->GetJavaVM(&vm) < 0) {
        env->ExceptionDescribe();
        return;
    }

    obj = env->NewGlobalRef(obj_);
    if (obj == nullptr) {
        env->ExceptionDescribe();
        return;
    }
}

NativeMapView::~NativeMapView() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::~NativeMapView");
    terminateContext();
    destroySurface();
    terminateDisplay();

    assert(vm != nullptr);
    assert(obj != nullptr);

    jint ret;
    JNIEnv* env = nullptr;
    ret = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (ret == JNI_OK) {
        env->DeleteGlobalRef(obj);
    } else {
        mbgl::Log::Debug(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
    }
    obj = nullptr;
    vm = nullptr;
}

bool NativeMapView::initializeDisplay() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::initializeDisplay");

    assert(display == EGL_NO_DISPLAY);
    assert(config == nullptr);
    assert(format < 0);

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetDisplay() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglInitialize() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }
    if ((major <= 1) && (minor < 3)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "EGL version is too low, need 1.3, got %d.%d", major, minor);
        terminateDisplay();
        return false;
    }

    log_egl_string(display, EGL_VENDOR, "Vendor");
    log_egl_string(display, EGL_VERSION, "Version");
    log_egl_string(display, EGL_CLIENT_APIS, "Client APIs");
    log_egl_string(display, EGL_EXTENSIONS, "Client Extensions");

    // Detect if we are in emulator
    char prop[PROP_VALUE_MAX];
    __system_property_get("ro.kernel.qemu", prop);
    bool in_emulator = strtol(prop, nullptr, 0) == 1;
    if (in_emulator) {
        mbgl::Log::Warning(mbgl::Event::Android, "In emulator! Enabling hacks :-(");
    }

    // Try 565 first (faster)
    bool use565 = true;
    EGLint config_attribs[] = {
        EGL_CONFIG_CAVEAT, EGL_NONE,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        //EGL_CONFORMANT, EGL_OPENGL_ES2_BIT, // Emulator does not support this, in here, but will return conformant anyway, what?
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        //EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER, // Emulator does not support this, what?
        EGL_BUFFER_SIZE, 16,
        EGL_RED_SIZE, 5,
        EGL_GREEN_SIZE, 6,
        EGL_BLUE_SIZE, 5,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
        (in_emulator ? EGL_NONE : EGL_CONFORMANT), EGL_OPENGL_ES2_BIT, // Ugly hack
        (in_emulator ? EGL_NONE : EGL_COLOR_BUFFER_TYPE), EGL_RGB_BUFFER, // Ugly hack
        EGL_NONE
    };
    EGLint num_configs;
    if (!eglChooseConfig(display, config_attribs, nullptr, 0, &num_configs)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglChooseConfig(NULL) returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }
    if (num_configs < 1) {
        mbgl::Log::Warning(mbgl::Event::OpenGL, "eglChooseConfig() returned no configs for 565");

        // Now try 8888
        use565 = false;
        config_attribs[5] = 32; // EGL_BUFFER_SIZE // Ensure we get 32bit color buffer on Tegra, 24 bit will be sorted first without it (slow software mode)
        config_attribs[7] = 8; // EGL_RED_SIZE
        config_attribs[9] = 8; // EGL_GREEN_SIZE
        config_attribs[11] = 8; // EGL_BLUE_SIZE
        if (!eglChooseConfig(display, config_attribs, nullptr, 0, &num_configs)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglChooseConfig(NULL) returned error %d", eglGetError());
            terminateDisplay();
            return false;
        }
        if (num_configs < 1) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglChooseConfig() returned no configs for 8888");
            terminateDisplay();
            return false;
        }
    }

    const std::unique_ptr<EGLConfig[]> configs = boost::make_unique<EGLConfig[]>(num_configs); //  switch to std::make_unique in C++14
    if (!eglChooseConfig(display, config_attribs, configs.get(), num_configs,
            &num_configs)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglChooseConfig() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }

    config = chooseConfig(configs.get(), num_configs, use565);
    if (config == nullptr) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "No config chosen");
        terminateDisplay();
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }
    mbgl::Log::Info(mbgl::Event::OpenGL, "Chosen window format is %d", format);

    return true;
}

void NativeMapView::terminateDisplay() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::terminateDisplay");

    if (display != EGL_NO_DISPLAY) {
        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                EGL_NO_CONTEXT)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                    eglGetError());
        }

        if (!eglTerminate(display)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglTerminate() returned error %d", eglGetError());
        }
    }

    display = EGL_NO_DISPLAY;
    config = nullptr;
    format = -1;
}

bool NativeMapView::initializeContext() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::initializeContext");

    assert(display != EGL_NO_DISPLAY);
    assert(context == EGL_NO_CONTEXT);
    assert(config != nullptr);

    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT,
            context_attribs);
    if (context == EGL_NO_CONTEXT) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglCreateContext() returned error %d", eglGetError());
        terminateContext();
        return false;
    }

    return true;
}

void NativeMapView::terminateContext() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::terminateContext");

    if (display != EGL_NO_DISPLAY) {
        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                EGL_NO_CONTEXT)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                    eglGetError());
        }

        if (context != EGL_NO_CONTEXT) {
            if (!eglDestroyContext(display, context)) {
                mbgl::Log::Error(mbgl::Event::OpenGL, "eglDestroyContext() returned error %d", eglGetError());
            }
        }
    }

    context = EGL_NO_CONTEXT;
}

bool NativeMapView::createSurface(ANativeWindow* window_) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::createSurface");

    assert(window == nullptr);
    assert(window_ != nullptr);
    window = window_;

    assert(display != EGL_NO_DISPLAY);
    assert(surface == EGL_NO_SURFACE);
    assert(config != nullptr);
    assert(format >= 0);

    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    const EGLint surface_attribs[] = {
        EGL_NONE
    };
    surface = eglCreateWindowSurface(display, config, window, surface_attribs);
    if (surface == EGL_NO_SURFACE) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglCreateWindowSurface() returned error %d", eglGetError());
        destroySurface();
        return false;
    }

    if (!first_time) {
        first_time = true;

        if (!eglMakeCurrent(display, surface, surface, context)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent() returned error %d", eglGetError());
        }

        log_gl_string(GL_VENDOR, "Vendor");
        log_gl_string(GL_RENDERER, "Renderer");
        log_gl_string(GL_VERSION, "Version");
        log_gl_string(GL_SHADING_LANGUAGE_VERSION, "SL Version"); // In the emulator this returns NULL with error code 0? https://code.google.com/p/android/issues/detail?id=78977
        log_gl_string(GL_EXTENSIONS, "Extensions");

        loadExtensions();

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                EGL_NO_CONTEXT)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                    eglGetError());
        }
    }

    resume();

    return true;
}

void NativeMapView::destroySurface() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::destroySurface");

    pause(true);

    if (surface != EGL_NO_SURFACE) {
        if (!eglDestroySurface(display, surface)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglDestroySurface() returned error %d", eglGetError());
        }
    }

    surface = EGL_NO_SURFACE;

    if (window != nullptr) {
        ANativeWindow_release(window);
        window = nullptr;
    }
}

EGLConfig NativeMapView::chooseConfig(const EGLConfig configs[],
        EGLint num_configs, bool use565) const {
    mbgl::Log::Info(mbgl::Event::OpenGL, "Found %d configs", num_configs);

    int chosen_config = -1;
    bool is_caveat = false;
    bool is_conformant = false;
    for (int i = 0; i < num_configs; i++) {
        mbgl::Log::Info(mbgl::Event::OpenGL, "Config %d:", i);

        EGLint caveat, conformant, bits, red, green, blue, alpha, alpha_mask, depth, stencil,
                sample_buffers, samples;

        if (!eglGetConfigAttrib(display, configs[i], EGL_CONFIG_CAVEAT, &caveat)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_CONFIG_CAVEAT) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_CONFORMANT, &conformant)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_CONFORMANT) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_BUFFER_SIZE, &bits)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_BUFFER_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_RED_SIZE, &red)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_RED_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_GREEN_SIZE, &green)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_GREEN_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_BLUE_SIZE, &blue)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_BLUE_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_ALPHA_SIZE, &alpha)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_ALPHA_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_ALPHA_MASK_SIZE,
                &alpha_mask)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_ALPHA_MASK_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_DEPTH_SIZE, &depth)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_DEPTH_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_STENCIL_SIZE,
                &stencil)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_STENCIL_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_SAMPLE_BUFFERS,
                &sample_buffers)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_SAMPLE_BUFFERS) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_SAMPLES, &samples)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_SAMPLES) returned error %d",
                    eglGetError());
            return nullptr;
        }

        mbgl::Log::Info(mbgl::Event::OpenGL, "Caveat: %d", caveat);
        mbgl::Log::Info(mbgl::Event::OpenGL, "Conformant: %d", conformant);
        mbgl::Log::Info(mbgl::Event::OpenGL, "Color: %d", bits);
        mbgl::Log::Info(mbgl::Event::OpenGL, "Red: %d", red);
        mbgl::Log::Info(mbgl::Event::OpenGL, "Green: %d", green);
        mbgl::Log::Info(mbgl::Event::OpenGL, "Blue: %d", blue);
        mbgl::Log::Info(mbgl::Event::OpenGL, "Alpha: %d", alpha);
        mbgl::Log::Info(mbgl::Event::OpenGL, "Alpha mask: %d", alpha_mask);
        mbgl::Log::Info(mbgl::Event::OpenGL, "Depth: %d", depth);
        mbgl::Log::Info(mbgl::Event::OpenGL, "Stencil: %d", stencil);
        mbgl::Log::Info(mbgl::Event::OpenGL, "Sample buffers: %d", sample_buffers);
        mbgl::Log::Info(mbgl::Event::OpenGL, "Samples: %d", samples);

        bool config_ok = true;
        config_ok &= bits == (use565 ? 16 : 32);
        config_ok &= red == (use565 ? 5 : 8);
        config_ok &= green == (use565 ? 6 : 8);
        config_ok &= blue == (use565 ? 5 : 8);
        config_ok &= (alpha == 0) || (alpha == 8); // Can be either 0 for RGBX or 8 for RGBA but we don't care either way
        config_ok &= (depth == 24) || (depth == 16);
        config_ok &= stencil == 8;
        config_ok &= sample_buffers == 0;
        config_ok &= samples == 0;

        if (config_ok) { // Choose the last matching config, that way we get RGBX if possible (since it is sorted highest to lowest bits)
            chosen_config = i;
            is_conformant = (conformant & EGL_OPENGL_ES2_BIT) == EGL_OPENGL_ES2_BIT;
            is_caveat = caveat != EGL_NONE;
        }
    }

    mbgl::Log::Debug(mbgl::Event::OpenGL, "Chosen config is %d", chosen_config);
    if (chosen_config >= 0) {
        return configs[chosen_config];
    } else {
        return nullptr;
    }
    if (!is_caveat) {
        mbgl::Log::Warning(mbgl::Event::OpenGL, "Chosen config has a caveat.");
    }
    if (!is_conformant) {
        mbgl::Log::Warning(mbgl::Event::OpenGL, "Chosen config is not conformant.");
    }
}

void NativeMapView::start() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::start");

    if (display == EGL_NO_DISPLAY) {
        initializeDisplay();
    }

    if (context == EGL_NO_CONTEXT) {
        initializeContext();
    }

    assert(display != EGL_NO_DISPLAY);
    assert(context != EGL_NO_CONTEXT);

    map.start(true);
}

void NativeMapView::loadExtensions() {
    const GLubyte* str = glGetString(GL_EXTENSIONS);
    if (str == nullptr) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "glGetString(GL_EXTENSIONS) returned error %d", glGetError());
        return;
    }

    std::string extensions(reinterpret_cast<const char*>(str));

    if (extensions.find("GL_OES_vertex_array_object") != std::string::npos) {
        mbgl::Log::Info(mbgl::Event::OpenGL, "Using GL_OES_vertex_array_object.");
        gl::BindVertexArray = (gl::PFNGLBINDVERTEXARRAYPROC)eglGetProcAddress("glBindVertexArrayOES");
        gl::DeleteVertexArrays = (gl::PFNGLDELETEVERTEXARRAYSPROC)eglGetProcAddress("glDeleteVertexArraysOES");
        gl::GenVertexArrays = (gl::PFNGLGENVERTEXARRAYSPROC)eglGetProcAddress("glGenVertexArraysOES");
        gl::IsVertexArray = (gl::PFNGLISVERTEXARRAYPROC)eglGetProcAddress("glIsVertexArrayOES");

        if ((gl::BindVertexArray == nullptr) || (gl::DeleteVertexArrays == nullptr) || (gl::GenVertexArrays == nullptr) || (gl::IsVertexArray == nullptr)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Could not load GL_OES_vertex_array_object functions.");
            gl::BindVertexArray = nullptr;
            gl::DeleteVertexArrays = nullptr;
            gl::GenVertexArrays = nullptr;
            gl::IsVertexArray = nullptr;
        }
    }
}

void NativeMapView::stop() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::stop");

    if ((display != EGL_NO_DISPLAY) && (display != EGL_NO_CONTEXT)) {
        map.stop();
    }
}


void NativeMapView::pause(bool wait_for_pause) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::pause %s", (wait_for_pause) ? "true" : "false");

    if ((display != EGL_NO_DISPLAY) && (display != EGL_NO_CONTEXT)) {
        map.pause(wait_for_pause);
    }
}

void NativeMapView::resume() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::resume");

    assert(display != EGL_NO_DISPLAY);
    assert(context != EGL_NO_CONTEXT);

    if (surface != EGL_NO_SURFACE) {
        map.resume();
    } else {
        mbgl::Log::Debug(mbgl::Event::Android, "Not resuming because we are not ready");
    }
}

void NativeMapView::notifyMapChange() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::notify_map_change()");

    assert(vm != nullptr);
    assert(obj != nullptr);

    JavaVMAttachArgs args = {
        JNI_VERSION_1_2,
        "NativeMapView::notify_map_change()",
        NULL
    };

    jint ret;
    JNIEnv* env = nullptr;
    bool detach = false;
    ret = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        if (ret != JNI_EDETACHED) {
            mbgl::Log::Error(mbgl::Event::JNI, "GetEnv() failed with %i", ret);
            return;
        } else {
            ret = vm->AttachCurrentThread(&env, &args);
            if (ret != JNI_OK) {
                mbgl::Log::Error(mbgl::Event::JNI, "AttachCurrentThread() failed with %i", ret);
                return;
            }
            detach = true;
        }
    }

    env->CallVoidMethod(obj, on_map_changed_id);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    if (detach) {
        if ((ret = vm->DetachCurrentThread()) != JNI_OK) {
            mbgl::Log::Error(mbgl::Event::JNI, "DetachCurrentThread() failed with %i", ret);
            return;
        }
    }
    env = nullptr;
}

}
}
