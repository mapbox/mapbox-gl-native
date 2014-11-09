#include <cstdlib>
#include <memory>

#include <sys/system_properties.h>

#include <GLES2/gl2.h>

#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/android/log_android.hpp>

#include "log.h"

#include "NativeMapView.hpp"

namespace mbgl {
namespace android {

void log_egl_string(EGLDisplay display, EGLint name, const char* label) {
    const char* str = eglQueryString(display, name);
    if (str == nullptr) {
        LOG_ERROR("eglQueryString(%d) returned error %d", name, eglGetError());
    } else {
        LOG_INFO("EGL %s: %s", label, str);
    }
}

void log_gl_string(GLenum name, const char* label) {
    const GLubyte* str = glGetString(name);
    if (str == nullptr) {
        LOG_ERROR("glGetString(%d) returned error %d", name, glGetError());
    } else {
        LOG_INFO("GL %s: %s", label, str);
    }
}

NativeMapView::NativeMapView(JNIEnv* env, jobject obj_) {
    LOG_VERBOSE("NativeMapView::NativeMapView");

    LOG_ASSERT(env != nullptr);
    LOG_ASSERT(obj_ != nullptr);

    if (env->GetJavaVM(&vm) < 0) {
        env->ExceptionDescribe();
        return;
    }

    obj = env->NewGlobalRef(obj_);
    if (obj == nullptr) {
        env->ExceptionDescribe();
        return;
    }

    mbgl::Log::Set<mbgl::AndroidLogBackend>();

    view = new MBGLView(this);
    map = new mbgl::Map(*view);
}

NativeMapView::~NativeMapView() {
    LOG_VERBOSE("NativeMapView::~NativeMapView");
    terminateContext();

    delete map;
    map = nullptr;

    delete view;
    view = nullptr;

    LOG_ASSERT(vm != nullptr);
    LOG_ASSERT(obj != nullptr);

    jint ret;
    JNIEnv* env = nullptr;
    if ((ret = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6)) == JNI_OK) {
        env->DeleteGlobalRef(obj);
    } else {
        LOG_ERROR("GetEnv() failed with %i", ret);
    }
    obj = nullptr;
    vm = nullptr;
}

bool NativeMapView::initializeDisplay() {
    LOG_VERBOSE("NativeMapView::initializeDisplay");

    LOG_ASSERT(display == EGL_NO_DISPLAY);
    LOG_ASSERT(config == nullptr);
    LOG_ASSERT(format < 0);

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOG_ERROR("eglGetDisplay() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        LOG_ERROR("eglInitialize() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }
    if ((major <= 1) && (minor < 3)) {
        LOG_ERROR("EGL version is too low, need 1.3, got %d.%d", major, minor);
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
    LOG_INFO("ro.kernel.qemu = %s", prop);
    if (in_emulator) {
        LOG_INFO("In emulator! Enabling hacks :-(");
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
        LOG_ERROR("eglChooseConfig(NULL) returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }
    if (num_configs < 1) {
        LOG_WARN("eglChooseConfig() returned no configs for 565");

        // Now try 8888
        use565 = false;
        config_attribs[5] = 32; // EGL_BUFFER_SIZE // Ensure we get 32bit color buffer on Tegra, 24 bit will be sorted first without it (slow software mode)
        config_attribs[7] = 8; // EGL_RED_SIZE
        config_attribs[9] = 8; // EGL_GREEN_SIZE
        config_attribs[11] = 8; // EGL_BLUE_SIZE
        if (!eglChooseConfig(display, config_attribs, nullptr, 0, &num_configs)) {
            LOG_ERROR("eglChooseConfig(NULL) returned error %d", eglGetError());
            terminateDisplay();
            return false;
        }
        if (num_configs < 1) {
            LOG_ERROR("eglChooseConfig() returned no configs for 8888");
            terminateDisplay();
            return false;
        }
    }

    // TODO make_unique missing in Android NDK?
//    const std::unique_ptr<EGLConfig[]> configs = std::make_unique(new EGLConfig[num_configs]);
    const std::unique_ptr<EGLConfig[]> configs(new EGLConfig[num_configs]);
    if (!eglChooseConfig(display, config_attribs, configs.get(), num_configs,
            &num_configs)) {
        LOG_ERROR("eglChooseConfig() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }

    config = chooseConfig(configs.get(), num_configs, use565);
    if (config == nullptr) {
        LOG_ERROR("No config chosen");
        terminateDisplay();
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOG_ERROR("eglGetConfigAttrib() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }
    LOG_DEBUG("Chosen window format is %d", format);

    return true;
}

void NativeMapView::terminateDisplay() {
    LOG_VERBOSE("NativeMapView::terminateDisplay");

    map->terminate();

    if (display != EGL_NO_DISPLAY) {
        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                EGL_NO_CONTEXT)) {
            LOG_ERROR("eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                    eglGetError());
        }

        if (!eglTerminate(display)) {
            LOG_ERROR("eglTerminate() returned error %d", eglGetError());
        }
    }

    display = EGL_NO_DISPLAY;
    config = nullptr;
    format = -1;
}

bool NativeMapView::initializeContext() {
    LOG_VERBOSE("NativeMapView::initializeContext");

    LOG_ASSERT(display != EGL_NO_DISPLAY);
    LOG_ASSERT(context == EGL_NO_CONTEXT);
    LOG_ASSERT(config != nullptr);

    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT,
            context_attribs);
    if (context == EGL_NO_CONTEXT) {
        LOG_ERROR("eglCreateContext() returned error %d", eglGetError());
        terminateContext();
        return false;
    }

    LOG_INFO("Context initialized");

    return true;
}

void NativeMapView::terminateContext() {
    LOG_VERBOSE("NativeMapView::terminateContext");

    map->terminate();

    // TODO: there is a bug when you double tap home to go app switcher, as map is black if you immediately switch to the map again
    // TODO: this is in the onPause/onResume path
    // TODO: the bug causes an GL_INVALID_VALUE with glDelteProgram (I think due to context being deleted first)
    // TODO: we need to free resources before we terminate
    // TODO: but cause terminate and stop is async they try to do stuff with no context and crash!

    if (display != EGL_NO_DISPLAY) {
        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                EGL_NO_CONTEXT)) {
            LOG_ERROR("eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                    eglGetError());
        }

        if (context != EGL_NO_CONTEXT) {
            if (!eglDestroyContext(display, context)) {
                LOG_ERROR("eglDestroyContext() returned error %d", eglGetError());
            }
        }
    }

    context = EGL_NO_CONTEXT;
}

bool NativeMapView::createSurface(ANativeWindow* window_) {
    LOG_VERBOSE("NativeMapView::createSurface");

    LOG_ASSERT(window == nullptr);
    LOG_ASSERT(window_ != nullptr);
    window = window_;

    LOG_ASSERT(display != EGL_NO_DISPLAY);
    LOG_ASSERT(surface == EGL_NO_SURFACE);
    LOG_ASSERT(config != nullptr);
    LOG_ASSERT(format >= 0);

    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    const EGLint surface_attribs[] = {
        EGL_NONE
    };
    surface = eglCreateWindowSurface(display, config, window, surface_attribs);
    if (surface == EGL_NO_SURFACE) {
        LOG_ERROR("eglCreateWindowSurface() returned error %d", eglGetError());
        destroySurface();
        return false;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOG_ERROR("eglMakeCurrent() returned error %d", eglGetError());
    }

    log_gl_string(GL_VENDOR, "Vendor");
    log_gl_string(GL_RENDERER, "Renderer");
    log_gl_string(GL_VERSION, "Version");
    log_gl_string(GL_SHADING_LANGUAGE_VERSION, "SL Version"); // In the emulator this returns NULL with error code 0? https://code.google.com/p/android/issues/detail?id=78977
    log_gl_string(GL_EXTENSIONS, "Extensions");

    loadExtensions();        

    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
            EGL_NO_CONTEXT)) {
        LOG_ERROR("eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                eglGetError());
    }

    resume();

    return true;
}

void NativeMapView::destroySurface() {
    LOG_VERBOSE("NativeMapView::destroySurface");

    pause(true);

    if (surface != EGL_NO_SURFACE) {
        if (!eglDestroySurface(display, surface)) {
            LOG_ERROR("eglDestroySurface() returned error %d", eglGetError());
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
    LOG_INFO("Found %d configs", num_configs);

    int chosen_config = -1;
    bool is_caveat = false;
    bool is_conformant = false;
    for (int i = 0; i < num_configs; i++) {
        LOG_INFO("Config %d:", i);

        EGLint caveat, conformant, bits, red, green, blue, alpha, alpha_mask, depth, stencil,
                sample_buffers, samples;

        if (!eglGetConfigAttrib(display, configs[i], EGL_CONFIG_CAVEAT, &caveat)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_CONFIG_CAVEAT) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_CONFORMANT, &conformant)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_CONFORMANT) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_BUFFER_SIZE, &bits)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_BUFFER_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_RED_SIZE, &red)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_RED_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_GREEN_SIZE, &green)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_GREEN_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_BLUE_SIZE, &blue)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_BLUE_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_ALPHA_SIZE, &alpha)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_ALPHA_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_ALPHA_MASK_SIZE,
                &alpha_mask)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_ALPHA_MASK_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_DEPTH_SIZE, &depth)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_DEPTH_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_STENCIL_SIZE,
                &stencil)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_STENCIL_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_SAMPLE_BUFFERS,
                &sample_buffers)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_SAMPLE_BUFFERS) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_SAMPLES, &samples)) {
            LOG_ERROR("eglGetConfigAttrib(EGL_SAMPLES) returned error %d",
                    eglGetError());
            return nullptr;
        }
        
        LOG_INFO("Caveat: %d", caveat);
        LOG_INFO("Conformant: %d", conformant);
        LOG_INFO("Color: %d", bits);
        LOG_INFO("Red: %d", red);
        LOG_INFO("Green: %d", green);
        LOG_INFO("Blue: %d", blue);
        LOG_INFO("Alpha: %d", alpha);
        LOG_INFO("Alpha mask: %d", alpha_mask);
        LOG_INFO("Depth: %d", depth);
        LOG_INFO("Stencil: %d", stencil);
        LOG_INFO("Sample buffers: %d", sample_buffers);
        LOG_INFO("Samples: %d", samples);

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

    LOG_DEBUG("Chosen config is %d", chosen_config);
    if (chosen_config >= 0) {
        return configs[chosen_config];
    } else {
        return nullptr;
    }
    if (!is_caveat) {
        LOG_WARN("Chosen config has a caveat.");
    }
    if (!is_conformant) {
        LOG_WARN("Chosen config is not conformant.");
    }
}

void NativeMapView::start() {
    LOG_VERBOSE("NativeMapView::start");

    if (display == EGL_NO_DISPLAY) {
        initializeDisplay();
    }

    if (context == EGL_NO_CONTEXT) {
        initializeContext();
    }

    LOG_ASSERT(display != EGL_NO_DISPLAY);
    LOG_ASSERT(context != EGL_NO_CONTEXT);
    
    map->start(true);
}

void NativeMapView::loadExtensions() {
    const GLubyte* str = glGetString(GL_EXTENSIONS);
    if (str == nullptr) {
        LOG_ERROR("glGetString(GL_EXTENSIONS) returned error %d", glGetError());
        return;
    }

    std::string extensions(reinterpret_cast<const char*>(str));
    
    if (extensions.find("GL_OES_vertex_array_object") != std::string::npos) {
        LOG_INFO("Using GL_OES_vertex_array_object.");
        gl::BindVertexArray = (gl::PFNGLBINDVERTEXARRAYPROC)eglGetProcAddress("glBindVertexArrayOES");
        gl::DeleteVertexArrays = (gl::PFNGLDELETEVERTEXARRAYSPROC)eglGetProcAddress("glDeleteVertexArraysOES");
        gl::GenVertexArrays = (gl::PFNGLGENVERTEXARRAYSPROC)eglGetProcAddress("glGenVertexArraysOES");
        gl::IsVertexArray = (gl::PFNGLISVERTEXARRAYPROC)eglGetProcAddress("glIsVertexArrayOES");

        if ((gl::BindVertexArray == nullptr) || (gl::DeleteVertexArrays == nullptr) || (gl::GenVertexArrays == nullptr) || (gl::IsVertexArray == nullptr)) {
            LOG_ERROR("Could not load GL_OES_vertex_array_object functions.");
            gl::BindVertexArray = nullptr;
            gl::DeleteVertexArrays = nullptr;
            gl::GenVertexArrays = nullptr;
            gl::IsVertexArray = nullptr;
        }
    }
}

void NativeMapView::stop() {
    LOG_VERBOSE("NativeMapView::stop");

    LOG_ASSERT(display != EGL_NO_DISPLAY);
    LOG_ASSERT(context != EGL_NO_CONTEXT);
    
    map->stop();
}


void NativeMapView::pause(bool wait_for_pause) {
    LOG_VERBOSE("NativeMapView::pause %s", (wait_for_pause) ? "true" : "false");

    LOG_ASSERT(display != EGL_NO_DISPLAY);
    LOG_ASSERT(context != EGL_NO_CONTEXT);
    
    map->pause(wait_for_pause);
}

void NativeMapView::resume() {
    LOG_VERBOSE("NativeMapView::resume");

    LOG_ASSERT(display != EGL_NO_DISPLAY);
    LOG_ASSERT(context != EGL_NO_CONTEXT);

    if (surface != EGL_NO_SURFACE) {
        map->resume();
    } else {
        LOG_DEBUG("Not resuming because we are not ready");
    }
}

void NativeMapView::notifyMapChange() {
    LOG_VERBOSE("NativeMapView::notify_map_change()");

    LOG_ASSERT(vm != nullptr);
    LOG_ASSERT(obj != nullptr);

    JavaVMAttachArgs args = {
        JNI_VERSION_1_2,
        "NativeMapView::notify_map_change()",
        NULL
    };

    // TODO do this only once per thread?
    jint ret;
    JNIEnv* env = nullptr;
    bool detach = false;
    if ((ret = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6)) != JNI_OK) {
        if (ret != JNI_EDETACHED) {
            LOG_ERROR("GetEnv() failed with %i", ret);
            return;
        } else {
            if ((ret = vm->AttachCurrentThread(&env, &args)) != JNI_OK) {
                LOG_ERROR("AttachCurrentThread() failed with %i", ret);
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
            LOG_ERROR("DetachCurrentThread() failed with %i", ret);
            return;
        }
    }
    env = nullptr;
}

void MBGLView::make_active() {
    LOG_VERBOSE("MBGLView::make_active");
    if ((nativeView->display != EGL_NO_DISPLAY)
            && (nativeView->surface != EGL_NO_SURFACE)
            && (nativeView->context != EGL_NO_CONTEXT)) {
        if (!eglMakeCurrent(nativeView->display, nativeView->surface,
                nativeView->surface, nativeView->context)) {
            LOG_ERROR("eglMakeCurrent() returned error %d", eglGetError());
        }
    } else {
        LOG_DEBUG("Not activating as we are not ready");
    }
}

void MBGLView::make_inactive() {
    LOG_VERBOSE("MBGLView::make_inactive");
    if (!eglMakeCurrent(nativeView->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
            EGL_NO_CONTEXT)) {
        LOG_ERROR("eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                eglGetError());
    }
}

void MBGLView::swap() {
    LOG_VERBOSE("MBGLView::swap");

    if (map->needsSwap() && (nativeView->display != EGL_NO_DISPLAY)
            && (nativeView->surface != EGL_NO_SURFACE)) {
        if (!eglSwapBuffers(nativeView->display, nativeView->surface)) {
            LOG_ERROR("eglSwapBuffers() returned error %d", eglGetError());
        }
        map->swapped();
    } else {
        LOG_DEBUG("Not swapping as we are not ready");
    }
}

void MBGLView::notify() {
    LOG_VERBOSE("MBGLView::notify()");
    // noop
}

void MBGLView::notify_map_change(mbgl::MapChange /* change */, mbgl::timestamp /* delay */) {
    LOG_VERBOSE("MBGLView::notify_map_change()");
    nativeView->notifyMapChange();
}

} // namespace android
} // namespace mbgl
