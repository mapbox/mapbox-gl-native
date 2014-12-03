#include <cstdlib>
#include <ctime>
#include <memory>
#include <list>
#include <tuple>

#include <sys/system_properties.h>

#include <GLES2/gl2.h>

#include <mbgl/android/native_map_view.hpp>
#include <mbgl/android/jni.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/event.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/std.hpp>

namespace mbgl {
namespace android {

void log_egl_string(EGLDisplay display, EGLint name, const char* label) {
    const char* str = eglQueryString(display, name);
    if (str == nullptr) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglQueryString(%d) returned error %d", name, eglGetError());
    } else {
        char buf[513];
        for (int len = std::strlen(str), pos = 0; len > 0; len -= 512, pos += 512) {
            strncpy(buf, str + pos, 512);
            buf[512] = 0;
            mbgl::Log::Info(mbgl::Event::OpenGL, "EGL %s: %s", label, buf);
        }
    }
}

void log_gl_string(GLenum name, const char* label) {
    const GLubyte* str = glGetString(name);
    if (str == nullptr) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "glGetString(%d) returned error %d", name, glGetError());
    } else {
        char buf[513];
        for (int len = std::strlen(reinterpret_cast<const char *>(str)), pos = 0; len > 0; len -= 512, pos += 512) {
            strncpy(buf, reinterpret_cast<const char *>(str) + pos, 512);
            buf[512] = 0;
            mbgl::Log::Info(mbgl::Event::OpenGL, "GL %s: %s", label, buf);
        }
    }
}

void MBGLView::make_active() {
    mbgl::Log::Debug(mbgl::Event::Android, "MBGLView::make_active");
    if ((nativeView.display != EGL_NO_DISPLAY) && (nativeView.surface != EGL_NO_SURFACE) && (nativeView.context != EGL_NO_CONTEXT)) {
        if (!eglMakeCurrent(nativeView.display, nativeView.surface, nativeView.surface, nativeView.context)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent() returned error %d", eglGetError());
        }
    } else {
        mbgl::Log::Info(mbgl::Event::Android, "Not activating as we are not ready");
    }
}

void MBGLView::make_inactive() {
    mbgl::Log::Debug(mbgl::Event::Android, "MBGLView::make_inactive");
    if (!eglMakeCurrent(nativeView.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d", eglGetError());
    }
}

void MBGLView::swap() {
    mbgl::Log::Debug(mbgl::Event::Android, "MBGLView::swap");

    if (map->needsSwap() && (nativeView.display != EGL_NO_DISPLAY) && (nativeView.surface != EGL_NO_SURFACE)) {
        if (!eglSwapBuffers(nativeView.display, nativeView.surface)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglSwapBuffers() returned error %d", eglGetError());
        }
        map->swapped();
        nativeView.updateFps();
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

    // Get all configs at least RGB 565 with 16 depth and 8 stencil
    EGLint config_attribs[] = {
        EGL_CONFIG_CAVEAT, EGL_NONE,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
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
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglChooseConfig() returned no configs.");
        terminateDisplay();
        return false;
    }

    const std::unique_ptr<EGLConfig[]> configs = mbgl::util::make_unique<EGLConfig[]>(num_configs);
    if (!eglChooseConfig(display, config_attribs, configs.get(), num_configs, &num_configs)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglChooseConfig() returned error %d", eglGetError());
        terminateDisplay();
        return false;
    }

    config = chooseConfig(configs.get(), num_configs);
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
        // Destroy the surface first, if it still exists. This call needs a valid surface.
        if (surface != EGL_NO_SURFACE) {
            if (!eglDestroySurface(display, surface)) {
                mbgl::Log::Error(mbgl::Event::OpenGL, "eglDestroySurface() returned error %d", eglGetError());
            }
            surface = EGL_NO_SURFACE;
        }

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
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
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
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
        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
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

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglMakeCurrent(EGL_NO_CONTEXT) returned error %d", eglGetError());
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

// Speed
/*
typedef enum {
    Format16Bit = 0,
    Format32BitNoAlpha = 1,
    Format32BitAlpha = 2,
    Format24Bit = 3,
    Unknown = 4
} BufferFormat;

typedef enum {
    Format16Depth8Stencil = 0,
    Format24Depth8Stencil = 1,
} DepthStencilFormat;
*/

// Quality
typedef enum {
    Format16Bit = 3,
    Format32BitNoAlpha = 1,
    Format32BitAlpha = 2,
    Format24Bit = 0,
    Unknown = 4
} BufferFormat;

typedef enum {
    Format16Depth8Stencil = 1,
    Format24Depth8Stencil = 0,
} DepthStencilFormat;

// Tuple is <buffer_format, depth_stencil_format, is_not_conformant, is_caveat, config_num, config_id>
typedef std::tuple<BufferFormat, DepthStencilFormat, bool, bool, int, EGLConfig> ConfigProperties;

EGLConfig NativeMapView::chooseConfig(const EGLConfig configs[], EGLint num_configs) {
    mbgl::Log::Info(mbgl::Event::OpenGL, "Found %d configs", num_configs);

    // Create a list of configs that pass our filters
    std::list<ConfigProperties> config_list;
    for (int i = 0; i < num_configs; i++) {
        mbgl::Log::Info(mbgl::Event::OpenGL, "Config %d:", i);

        EGLint caveat, conformant, bits, red, green, blue, alpha, alpha_mask, depth, stencil, sample_buffers, samples;

        if (!eglGetConfigAttrib(display, configs[i], EGL_CONFIG_CAVEAT, &caveat)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_CONFIG_CAVEAT) returned error %d", eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_CONFORMANT, &conformant)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_CONFORMANT) returned error %d", eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_BUFFER_SIZE, &bits)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_BUFFER_SIZE) returned error %d", eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_RED_SIZE, &red)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_RED_SIZE) returned error %d", eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_GREEN_SIZE, &green)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_GREEN_SIZE) returned error %d", eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_BLUE_SIZE, &blue)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_BLUE_SIZE) returned error %d", eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_ALPHA_SIZE, &alpha)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_ALPHA_SIZE) returned error %d", eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_ALPHA_MASK_SIZE, &alpha_mask)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_ALPHA_MASK_SIZE) returned error %d", eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_DEPTH_SIZE, &depth)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_DEPTH_SIZE) returned error %d", eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_STENCIL_SIZE, &stencil)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_STENCIL_SIZE) returned error %d", eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_SAMPLE_BUFFERS, &sample_buffers)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_SAMPLE_BUFFERS) returned error %d", eglGetError());
            return nullptr;
        }

        if (!eglGetConfigAttrib(display, configs[i], EGL_SAMPLES, &samples)) {
            mbgl::Log::Error(mbgl::Event::OpenGL, "eglGetConfigAttrib(EGL_SAMPLES) returned error %d", eglGetError());
            return nullptr;
        }

        mbgl::Log::Info(mbgl::Event::OpenGL, "...Caveat: %d", caveat);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Conformant: %d", conformant);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Color: %d", bits);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Red: %d", red);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Green: %d", green);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Blue: %d", blue);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Alpha: %d", alpha);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Alpha mask: %d", alpha_mask);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Depth: %d", depth);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Stencil: %d", stencil);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Sample buffers: %d", sample_buffers);
        mbgl::Log::Info(mbgl::Event::OpenGL, "...Samples: %d", samples);

        bool config_ok = true;
        config_ok &= (depth == 24) || (depth == 16);
        config_ok &= stencil == 8;
        config_ok &= sample_buffers == 0;
        config_ok &= samples == 0;

        // Filter our configs first for depth, stencil and anti-aliasing
        if (config_ok) {
            // Work out the config's buffer format
            BufferFormat buffer_format;
            if ((bits == 16) && (red == 5) && (green == 6) && (blue == 5) && (alpha == 0)) {
                buffer_format = Format16Bit;
            } else if ((bits == 32) && (red == 8) && (green == 8) && (blue == 8) && (alpha == 0)) {
                buffer_format = Format32BitNoAlpha;
            } else if ((bits == 32) && (red == 8) && (green == 8) && (blue == 8) && (alpha == 8)) {
                buffer_format = Format32BitAlpha;
            } else if ((bits == 24) && (red == 8) && (green == 8) && (blue == 8) && (alpha == 0)) {
                buffer_format = Format24Bit;
            } else {
                buffer_format = Unknown;
            }

            // Work out the config's depth stencil format
            DepthStencilFormat depth_stencil_format;
            if ((depth == 16) && (stencil == 8)) {
                depth_stencil_format = Format16Depth8Stencil;
            } else {
                depth_stencil_format = Format24Depth8Stencil;
            }

            bool is_not_conformant = (conformant & EGL_OPENGL_ES2_BIT) != EGL_OPENGL_ES2_BIT;
            bool is_caveat = caveat != EGL_NONE;
            EGLConfig config_id = configs[i];

            // Ignore formats we don't recognise
            if (buffer_format != Unknown)
            {
                config_list.push_back(std::make_tuple(buffer_format, depth_stencil_format, is_not_conformant, is_caveat, i, config_id));
            }
        }
    }

    if (config_list.empty()) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "Config list was empty.");
    }

    // Sort the configs to find the best one
    config_list.sort();
    using_depth24 = std::get<1>(config_list.front()) == Format24Depth8Stencil;
    bool is_conformant = !std::get<2>(config_list.front());
    bool is_caveat = std::get<3>(config_list.front());
    int config_num = std::get<4>(config_list.front());
    EGLConfig config_id = std::get<5>(config_list.front());

    mbgl::Log::Info(mbgl::Event::OpenGL, "Chosen config is %d", config_num);

    if (is_caveat) {
        mbgl::Log::Warning(mbgl::Event::OpenGL, "Chosen config has a caveat.");
    }
    if (!is_conformant) {
        mbgl::Log::Warning(mbgl::Event::OpenGL, "Chosen config is not conformant.");
    }

    return config_id;
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

    if (extensions.find("GL_OES_packed_depth_stencil") != std::string::npos) {
        mbgl::Log::Info(mbgl::Event::OpenGL, "Using GL_OES_packed_depth_stencil.");
        gl::is_packed_depth_stencil_supported = true;
    }

    if (extensions.find("GL_OES_depth24") != std::string::npos) {
        mbgl::Log::Info(mbgl::Event::OpenGL, "Using GL_OES_depth24.");
        if (using_depth24) {
            gl::is_depth24_supported = true;
        } else {
            mbgl::Log::Info(mbgl::Event::OpenGL, "Preferring 16 bit depth.");
        }
    }
}

void NativeMapView::stop() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::stop");

    if ((display != EGL_NO_DISPLAY) && (context != EGL_NO_CONTEXT)) {
        map.stop();
    }
}


void NativeMapView::pause(bool wait_for_pause) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::pause %s", (wait_for_pause) ? "true" : "false");

    if ((display != EGL_NO_DISPLAY) && (context != EGL_NO_CONTEXT)) {
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

void NativeMapView::enableFps(bool enable) {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::enableFps()");

    fps_enabled = enable;
}

void NativeMapView::updateFps() {
    mbgl::Log::Debug(mbgl::Event::Android, "NativeMapView::updateFps()");

    if (!fps_enabled) {
        return;
    }

    static int frames = 0;
    static int64_t time_elapsed = 0LL;

    frames++;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    int64_t current_time = now.tv_sec*1000000000LL + now.tv_nsec;

    if (current_time - time_elapsed >= 1) {
        fps = frames / ((current_time - time_elapsed) / 1E9);
        mbgl::Log::Debug(mbgl::Event::Render, "FPS: %4.2f", fps);
        time_elapsed = current_time;
        frames = 0;
    }

    assert(vm != nullptr);
    assert(obj != nullptr);

    JavaVMAttachArgs args = {
        JNI_VERSION_1_2,
        "NativeMapView::updateFps()",
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

    env->CallVoidMethod(obj, on_fps_changed_id, fps);
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
