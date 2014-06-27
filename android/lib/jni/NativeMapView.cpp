//#include <stdio.h>

#include <memory>

#include <GLES2/gl2.h>

#include <llmr/platform/platform.hpp>

#include "log.h"

#include "NativeMapView.hpp"

namespace llmr {
namespace android {

void log_egl_string(EGLDisplay display, EGLint name, const char* label) {
    const char* str = eglQueryString(display, name);
    if (str == nullptr) {
        ERROR("eglQueryString(%d) returned error %d", name, eglGetError());
    } else {
        INFO("EGL %s: %s", label, str);
    }
}

void log_gl_string(GLenum name, const char* label) {
    const GLubyte* str = glGetString(name);
    if (str == nullptr) {
        ERROR("glGetString(%d) returned error %d", name, glGetError());
    } else {
        INFO("GL %s: %s", label, str);
    }
}

NativeMapView::NativeMapView(JNIEnv* env, jobject obj,
        std::string default_style_json) :
        default_style_json(default_style_json) {
    VERBOSE("NativeMapView::NativeMapView");

    ASSERT(env != nullptr);
    ASSERT(obj != nullptr);

    if (env->GetJavaVM(&vm) < 0) {
        env->ExceptionDescribe();
        return;
    }

    this->obj = env->NewGlobalRef(obj);
    if (this->obj == nullptr) {
        env->ExceptionDescribe();
        return;
    }

    // TODO replace all printfs in map code with android logging
    //freopen("/sdcard/stdout.txt", "w", stdout); // NOTE: can't use <cstdio> till NDK fix the stdout macro bug
    //freopen("/sdcard/stderr.txt", "w", stderr);

    view = new LLMRView(this);
    map = new llmr::Map(*view);

    map->setStyleJSON(default_style_json);
}

NativeMapView::~NativeMapView() {
    VERBOSE("NativeMapView::~NativeMapView");
    terminateContext();

    delete map;
    map = nullptr;

    delete view;
    view = nullptr;

    ASSERT(vm != nullptr);
    ASSERT(obj != nullptr);

    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_OK) {
        env->DeleteGlobalRef(obj);
    } else {
        env->ExceptionDescribe();
    }
    obj = nullptr;
    vm = nullptr;
}

bool NativeMapView::initializeDisplay() {
    VERBOSE("NativeMapView::initializeDisplay");

    ASSERT(display == EGL_NO_DISPLAY);
    ASSERT(config == nullptr);
    ASSERT(format < 0);

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        ERROR("eglGetDisplay() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        ERROR("eglInitialize() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }
    if ((major <= 1) && (minor < 3)) {
        ERROR("EGL version is too low, need 1.3, got %d.%d", major, minor);
        terminateDisplay();
        return false;
    }

    log_egl_string(display, EGL_VENDOR, "Vendor");
    log_egl_string(display, EGL_VERSION, "Version");
    log_egl_string(display, EGL_CLIENT_APIS, "Client APIs");
    log_egl_string(display, EGL_EXTENSIONS, "Client Extensions");

    // TODO should try 565, then 8888?
    bool use565 = true;

    const EGLint config_attribs[] = {
        EGL_CONFIG_CAVEAT, EGL_NONE,
        EGL_CONFORMANT, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
        EGL_BUFFER_SIZE, use565 ? 16 : 32, // Ensure we get 32bit color buffer on Tegra, 24 bit will be sorted first without it (slow software mode)
        EGL_RED_SIZE, use565 ? 5 : 8,
        EGL_GREEN_SIZE, use565 ? 6 : 8,
        EGL_BLUE_SIZE, use565 ? 5 : 8,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    EGLint num_configs;
    if (!eglChooseConfig(display, config_attribs, nullptr, 0, &num_configs)) {
        ERROR("eglChooseConfig(NULL) returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }
    if (num_configs < 1) {
        ERROR("eglChooseConfig() returned no configs");
        terminateDisplay();
        return false;
    }

    // TODO make_unique missing?
//    const std::unique_ptr<EGLConfig[]> configs = std::make_unique(new EGLConfig[num_configs]);
    const std::unique_ptr<EGLConfig[]> configs(new EGLConfig[num_configs]);
    if (!eglChooseConfig(display, config_attribs, configs.get(), num_configs,
            &num_configs)) {
        ERROR("eglChooseConfig() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }

    config = chooseConfig(configs.get(), num_configs, use565);
    if (config == nullptr) {
        ERROR("No config chosen");
        terminateDisplay();
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        ERROR("eglGetConfigAttrib() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }
    DEBUG("Chosen window format is %d", format);

    return true;
}

void NativeMapView::terminateDisplay() {
    VERBOSE("NativeMapView::terminateDisplay");

    if (display != EGL_NO_DISPLAY) {
        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                EGL_NO_CONTEXT)) {
            ERROR("eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                    eglGetError());
        }

        if (!eglTerminate(display)) {
            ERROR("eglTerminate() returned error %d", eglGetError());
        }
    }

    display = EGL_NO_DISPLAY;
    config = nullptr;
    format = -1;
}

bool NativeMapView::initializeContext() {
    VERBOSE("NativeMapView::initializeContext");

    ASSERT(display != EGL_NO_DISPLAY);
    ASSERT(context == EGL_NO_CONTEXT);
    ASSERT(config != nullptr);

    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT,
            context_attribs);
    if (context == EGL_NO_CONTEXT) {
        ERROR("eglCreateContext() returned error %d", eglGetError());
        terminateContext();
        return false;
    }

    INFO("Context initialized");

    return true;
}

void NativeMapView::terminateContext() {
    VERBOSE("NativeMapView::terminateContext");

    map->terminate();

    // TODO: there is a bug when you double tap home to go app switcher, as map is black if you immediately switch to the map again
    // TODO: this is in the onPause/onResume path
    // TODO: the bug causes an GL_INVALID_VALUE with glDelteProgram (I think due to context being deleted first)
    // TODO: we need to free resources before we terminate
    // TODO: but cause terminate and stop is async they try to do stuff with no context and crash!

    if (display != EGL_NO_DISPLAY) {
        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                EGL_NO_CONTEXT)) {
            ERROR("eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                    eglGetError());
        }

        if (context != EGL_NO_CONTEXT) {
            if (!eglDestroyContext(display, context)) {
                ERROR("eglDestroyContext() returned error %d", eglGetError());
            }
        }
    }

    context = EGL_NO_CONTEXT;
}

bool NativeMapView::createSurface(ANativeWindow* window) {
    VERBOSE("NativeMapView::createSurface");

    ASSERT(this->window == nullptr);
    ASSERT(window != nullptr);
    this->window = window;

    ASSERT(display != EGL_NO_DISPLAY);
    ASSERT(surface == EGL_NO_SURFACE);
    ASSERT(config != nullptr);
    ASSERT(format >= 0);

    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    const EGLint surface_attribs[] = {
        EGL_NONE
    };
    surface = eglCreateWindowSurface(display, config, window, surface_attribs);
    if (surface == EGL_NO_SURFACE) {
        ERROR("eglCreateWindowSurface() returned error %d", eglGetError());
        destroySurface();
        return false;
    }

    start();

    return true;
}

void NativeMapView::destroySurface() {
    VERBOSE("NativeMapView::destroySurface");

    if (surface != EGL_NO_SURFACE) {
        if (!eglDestroySurface(display, surface)) {
            ERROR("eglDestroySurface() returned error %d", eglGetError());
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
    INFO("Found %d configs", num_configs);

    int chosen_config = -1;
    for (int i = 0; i < num_configs; i++) {
        INFO("Config %d:", i);

        EGLint bits, red, green, blue, alpha, alpha_mask, depth, stencil,
                sample_buffers, samples;

        if (!eglGetConfigAttrib(display, configs[i], EGL_BUFFER_SIZE, &bits)) {
            ERROR("eglGetConfigAttrib(EGL_BUFFER_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_RED_SIZE, &red)) {
            ERROR("eglGetConfigAttrib(EGL_RED_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_GREEN_SIZE, &green)) {
            ERROR("eglGetConfigAttrib(EGL_GREEN_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_BLUE_SIZE, &blue)) {
            ERROR("eglGetConfigAttrib(EGL_BLUE_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_ALPHA_SIZE, &alpha)) {
            ERROR("eglGetConfigAttrib(EGL_ALPHA_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_ALPHA_MASK_SIZE,
                &alpha_mask)) {
            ERROR("eglGetConfigAttrib(EGL_ALPHA_MASK_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_DEPTH_SIZE, &depth)) {
            ERROR("eglGetConfigAttrib(EGL_DEPTH_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_STENCIL_SIZE,
                &stencil)) {
            ERROR("eglGetConfigAttrib(EGL_STENCIL_SIZE) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_SAMPLE_BUFFERS,
                &sample_buffers)) {
            ERROR("eglGetConfigAttrib(EGL_SAMPLE_BUFFERS) returned error %d",
                    eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_SAMPLES, &samples)) {
            ERROR("eglGetConfigAttrib(EGL_SAMPLES) returned error %d",
                    eglGetError());
            return nullptr;
        }

        INFO("Color: %d", bits);
        INFO("Red: %d", red);
        INFO("Green: %d", green);
        INFO("Blue: %d", blue);
        INFO("Alpha: %d", alpha);
        INFO("Alpha mask: %d", alpha_mask);
        INFO("Depth: %d", depth);
        INFO("Stencil: %d", stencil);
        INFO("Sample buffers: %d", sample_buffers);
        INFO("Samples: %d", samples);

        bool config_ok = true;
        config_ok &= bits == (use565 ? 16 : 32);
        config_ok &= red == (use565 ? 5 : 8);
        config_ok &= green == (use565 ? 6 : 8);
        config_ok &= blue == (use565 ? 5 : 8);
        //config_ok &= (alpha == 0) || (alpha == 8); // Can be either 0 for RGBX or 8 for RGBA but we don't care either way
        config_ok &= alpha == 0; // TODO should prefer no alpha over alpha, and 565 over 8888?
        //config_ok &= depth == 16;
        //config_ok &= stencil == 8;
        //config_ok &= sample_buffers == 0;
        config_ok &= samples == 0;

        if (config_ok) { // Choose the last matching config, that way we get RGBX if possible (since it is sorted highest to lowest bits)
            chosen_config = i;
        }
    }

    DEBUG("Chosen config is %d", chosen_config);
    if (chosen_config >= 0) {
        return configs[chosen_config];
    } else {
        return nullptr;
    }
}

void NativeMapView::start() {
    VERBOSE("NativeMapView::start");

    if (display == EGL_NO_DISPLAY) {
        initializeDisplay();
    }

    if (context == EGL_NO_CONTEXT) {
        initializeContext();
    }

    if ((display != EGL_NO_DISPLAY) && (surface != EGL_NO_SURFACE)
            && (context != EGL_NO_CONTEXT)) {

        if (!eglMakeCurrent(display, surface, surface, context)) {
            ERROR("eglMakeCurrent() returned error %d", eglGetError());
        }

        log_gl_string(GL_VENDOR, "Vendor");
        log_gl_string(GL_RENDERER, "Renderer");
        log_gl_string(GL_VERSION, "Version");
        log_gl_string(GL_SHADING_LANGUAGE_VERSION, "SL Version");
        log_gl_string(GL_EXTENSIONS, "Extensions");

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                EGL_NO_CONTEXT)) {
            ERROR("eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                    eglGetError());
        }

        map->start();
    } else {
        DEBUG("Not starting because we are not ready");
    }
}

void NativeMapView::stop() {
    VERBOSE("NativeMapView::stop");
    if ((display != EGL_NO_DISPLAY) && (surface != EGL_NO_SURFACE)
            && (context != EGL_NO_CONTEXT)) {
        map->stop();
    }
}

void NativeMapView::notifyMapChange() {
    DEBUG("NativeMapView::notify_map_change()");

    ASSERT(vm != nullptr);
    ASSERT(obj != nullptr);

    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        env->ExceptionDescribe();
        return;
    }

    env->CallVoidMethod(obj, on_map_changed_id);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return;
    }
}

void LLMRView::make_active() {
    VERBOSE("LLMRView::make_active");
    if ((nativeView->display != EGL_NO_DISPLAY)
            && (nativeView->surface != EGL_NO_SURFACE)
            && (nativeView->context != EGL_NO_CONTEXT)) {
        if (!eglMakeCurrent(nativeView->display, nativeView->surface,
                nativeView->surface, nativeView->context)) {
            ERROR("eglMakeCurrent() returned error %d", eglGetError());
        }

        // TODO still can't fix the black screen problem :-(
        EGLint width, height;
        if (!eglQuerySurface(nativeView->display, nativeView->surface, EGL_WIDTH, &width)
                || !eglQuerySurface(nativeView->display, nativeView->surface, EGL_HEIGHT, &height)) {
            ERROR("eglQuerySurface() returned error %d", eglGetError());
        }
        map->resize(width, height);

    } else {
        DEBUG("Not activating as we are not ready");
    }
}

void LLMRView::make_inactive() {
    VERBOSE("LLMRView::make_inactive");
    if (!eglMakeCurrent(nativeView->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
            EGL_NO_CONTEXT)) {
        ERROR("eglMakeCurrent(EGL_NO_CONTEXT) returned error %d",
                eglGetError());
    }
}

void LLMRView::swap() {
    VERBOSE("LLMRView::swap");
    if (map->needsSwap() && (nativeView->display != EGL_NO_DISPLAY)
            && (nativeView->surface != EGL_NO_SURFACE)) {
        if (!eglSwapBuffers(nativeView->display, nativeView->surface)) {
            ERROR("eglSwapBuffers() returned error %d", eglGetError());
        }
        map->swapped();
    } else {
        DEBUG("Not swapping as we are not ready");
    }
}

void LLMRView::notify_map_change() {
    DEBUG("LLMRView::notify_map_change()");
    nativeView->notifyMapChange();
}

} // namespace android
} // namespace llmr
